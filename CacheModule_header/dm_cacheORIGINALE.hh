#pragma once

#include "orchestrator/module.h"
#include "orchestrator/structures.h"
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

/* Examples for declare an instance of cache with a size line of 64 bytes and
 * the total size equal to 4096 bytes. This parameter must be known at compile
 * time.
 * dm_cache::Cache<64, 4096> myCache("cacheName");
 *
 * Due to the inheritance of the module class the cache constructor it's
 * possible to add the priority field, the default value is 0.
 * dm_cache::Cache<64, 4096> myCache("cacheName", 10);
 *
 * To use the cache is available the onNotify method that accept a message that
 * has to contain a pointer to a cacheMessage in the magic_struct field. */

namespace dm_cache {

#define CACHE_MESSAGE(m) (static_cast<cacheMessage *>(m->magic_struct))
#define MEM_LINE (16)
#define ADDR_SIZE (16)

/* This offset is required due to the size of line memory. Even if the odd
 * address are not used, the memory is addressed with an address that have the
 * first bit equal to 0. Eventually this offset is necessary due for the cache
 * addressing */
#define MEM_OFFSET (1)

// The type of operation that can be requested to the direct mapped cache
typedef enum {
    SET_DIRTY,
    CHECK_VALIDITY_DIRTY,
    CHECK_DATA_VALIDITY,
    INVALID_LINE,
    LOAD,
    STORE
} oper_t;

/* The following struct defines which kind of parameters are required to perform
 * direct mapped cache operation.
 * Some examples of possible message accepted by the cache are:
 * dm_cache::cacheMessage(dm_cache::SET_DIRTY, address, true);
 *
 * Check if the bit validity of a specified address is set to true and both tags
 * match
 * dm_cache::cacheMessage(dm_cache::CHECK_DATA_VALIDITY, address);
 *
 * Suppose a cache line of 64 bytes the following operation stores a cache line
 * of 0 to a specified address.
 * std::vector<uint16_t> d(4, 0); // 64 bytes set to 0.
 * dm_cache::cacheMessage(dm_cache::STORE, address, true, d);
 */
struct cacheMessage {
    oper_t op;     // Type of requested operation
    uint16_t addr; // Address on which perform the operation
    bool arg;      // in/out value that depends on the requested operation

    /* Cache line of both load and store operation. This field must be filled in
     * STORE and LOAD operation */
    std::vector<uint16_t> data;

    cacheMessage(oper_t op, uint16_t addr, bool arg = true,
                 std::vector<uint16_t> data = std::vector<uint16_t>())
        : op(op), addr(addr), arg(arg), data(data){};

    // Default copy costructor
    cacheMessage(const cacheMessage &c) = default;
};

// Information about a single cache line with get and set operation of the field
class info_t {
    bool validity;
    bool dirty;
    uint16_t tag;

  public:
    void set_validity(bool v) { this->validity = v; }
    void set_dirty(bool d) { this->dirty = d; }
    void set_tag(uint16_t t) { this->tag = t; }

    bool get_validity() { return this->validity; }
    bool get_dirty() { return this->dirty; }
    uint16_t get_tag() { return this->tag; }
};

template <size_t size_line, size_t size_cache> class Cache : public module {
    const uint8_t offset_size;
    const uint8_t index_size;
    const uint8_t tag_size;

    std::vector<std::vector<uint16_t>> data;
    std::vector<info_t> tag_memory;

    // Get the index from a given address
    uint16_t resolve_index(uint16_t addr) {
        uint16_t mask = 0 | ((1 << (this->index_size + this->offset_size)) - 1);
        return (addr & mask) >> this->offset_size;
    }

    // Get the tag from a given address
    uint16_t resolve_tag(uint16_t addr) {
        return addr >> (this->index_size + this->offset_size);
    }

    // Given an address, set the bit dirty
    void set_dirty(uint16_t addr, bool d) {
        this->tag_memory.at(this->resolve_index(addr)).set_dirty(d);
    }

    // Given an address, set the bit validity
    void set_validity(uint16_t addr, bool v) {
        this->tag_memory.at(this->resolve_index(addr)).set_validity(v);
    }

    // Given an address, extract the tag and set it into tag memory
    void set_tag(uint16_t addr) {
        this->tag_memory.at(this->resolve_index(addr))
            .set_tag(this->resolve_tag(addr));
    }

    // Check if the data at a given address is valid and the tag matches with
    // the address
    bool check_data_validity(uint16_t addr) {
        return this->tag_memory.at(this->resolve_index(addr)).get_validity() &&
               this->tag_memory.at(this->resolve_index(addr)).get_tag() ==
                   this->resolve_tag(addr);
    }

    // Given an address, return if the data is valid and bit dirty is set
    bool check_validity_dirty(uint16_t addr) {
        return this->tag_memory.at(this->resolve_index(addr)).get_dirty() &&
               this->tag_memory.at(this->resolve_index(addr)).get_validity();
    }

    // Given an address, return the tag from the tag memory
    uint16_t get_tag(uint16_t addr) {
        return this->tag_memory.at(this->resolve_index(addr)).get_tag();
    }

    // Sets validity and dirty bits to 0 for the given address
    void invalid_line(uint16_t addr) {
        this->tag_memory.at(this->resolve_index(addr)).set_validity(false);
        this->tag_memory.at(this->resolve_index(addr)).set_dirty(false);
    }

    // Given an address return the data in the cache line to the corresponding
    // index
    std::vector<uint16_t> load(uint16_t addr) {
        return this->data.at(this->resolve_index(addr));
    }

    // Given address and data block store the latter to the corresponding index,
    // set the validity bit and the tag
    bool store(uint16_t addr, std::vector<uint16_t> data_block) {
        try {
            if (size_line / MEM_LINE != data_block.size())
                throw "The data passed to the STORE operation does not fill "
                      "the size of cache line";

            this->data.at(this->resolve_index(addr)) = data_block;
            this->set_tag(addr);
            this->set_validity(addr, true);
            return true;

        } catch (const char *err) {
            std::cerr << err << std::endl;
            return false;
        }
    }

  public:
    Cache(char const *name, int priority = 0)
        : module(name, priority),
          offset_size(std::log(size_line / MEM_LINE) / std::log(2) +
                      MEM_OFFSET),
          index_size(std::log(size_cache / size_line) / std::log(2)),
          tag_size(ADDR_SIZE - (offset_size + index_size)),
          data(size_cache / size_line,
               std::vector<uint16_t>(size_line / MEM_LINE)),
          tag_memory(size_cache / size_line) {

        // Check if the cache line size is power of 2.
        static_assert(
            (int)(log(size_line) / log(2)) == (log(size_line) / log(2)) ||
                size_line < MEM_LINE,
            "Cache line must be a power of 2 NOT less than the memory line.");

        // Check if the cache size is multiple of cache line.
        static_assert(
            (size_cache / size_line) == ((float)size_cache / size_line),
            "The size of cache MUST be X times the size of cache line.");
    };

    void onNotify(message *m) {
        if (getName() != std::string(m->dest))
            return;

        message *replyMsg = new message;
        cacheMessage *to_src = new cacheMessage(*CACHE_MESSAGE(m));

        replyMsg->magic_struct = static_cast<void *>(to_src);
        std::strcpy(replyMsg->source, m->dest);
        std::strcpy(replyMsg->dest, m->source);

        switch (CACHE_MESSAGE(m)->op) {
        case SET_DIRTY:
            this->set_dirty(CACHE_MESSAGE(m)->addr, CACHE_MESSAGE(m)->arg);
            to_src->arg = true;
            break;
        case CHECK_VALIDITY_DIRTY:
            to_src->arg = this->check_validity_dirty(CACHE_MESSAGE(m)->addr);
            break;
        case CHECK_DATA_VALIDITY:
            to_src->arg = this->check_data_validity(CACHE_MESSAGE(m)->addr);
            break;
        case INVALID_LINE:
            this->invalid_line(CACHE_MESSAGE(m)->addr);
            break;
        case LOAD:
            to_src->data = this->load(CACHE_MESSAGE(m)->addr);
            break;
        case STORE:
            to_src->arg =
                this->store(CACHE_MESSAGE(m)->addr, CACHE_MESSAGE(m)->data);
            break;
        default:
            break;
        }

#ifdef DM_CACHE_TESTING
        *m = *replyMsg;
        delete replyMsg;
#else
        sendWithDelay(replyMsg);
#endif
    }
};
} // namespace dm_cache
