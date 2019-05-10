#ifndef CACHE_WRITE_POLICIES
#define CACHE_WRITE_POLICIES

#include <cstdint>

#include "orchestrator/structures.h"
#include "orchestrator/module.h"
#inlcude "dm_cache.hh"

#include "SetAssociative_WritePolicies_message.h"

enum HIT_POLICY {WRITE_BACK, WRITE_THROUGH};
enum MISS_POLICY {WRITE_ALLOCATE, WRITE_NO_ALLOCATE};


class CacheWritePolicies : public Cache, public module {
	
	HIT_POLICY hit_policy;
	MISS_POLICY miss_policy;
	
	
		
public:

	CacheWritePolicies(string name, int priority, uint16_t cache_size, uint16_t line_size, HIT_POLICY hp, MISS_POLICY mp);
	void onNotify(message *m);	
}






#endif //CACHE_WRITE_POLICIES
