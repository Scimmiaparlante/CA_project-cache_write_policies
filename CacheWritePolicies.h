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
	
	message* WP_create_message(std::string destination);
	
	//functions implementation
	CWP_to_SAC* WP_set_dirty(SAC_to_CWP* request_struct);
	CWP_to_SAC* WP_check_validity_dirty(SAC_to_CWP* request_struct);
	CWP_to_SAC* WP_check_data_validity(SAC_to_CWP* request_struct);
	CWP_to_SAC* WP_check_dirty(SAC_to_CWP* request_struct);
	CWP_to_SAC* WP_invalid_line(SAC_to_CWP* request_struct);
	CWP_to_SAC* WP_load(SAC_to_CWP* request_struct);
	CWP_to_SAC* WP_store(SAC_to_CWP* request_struct);
	CWP_to_SAC* WP_write_with_policies(SAC_to_CWP* request_struct);
		
public:

	CacheWritePolicies(string name, int priority, uint16_t cache_size, uint16_t line_size, HIT_POLICY hp, MISS_POLICY mp);
	void onNotify(message *m);	
}






#endif //CACHE_WRITE_POLICIES
