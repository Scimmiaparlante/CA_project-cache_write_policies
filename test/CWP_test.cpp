#include<cstring>
#include "CacheWritePolicies.h"

void store(CacheWritePolicies c, const char *s){
	SAC_to_CWP	*request = new SAC_to_CWP;
	message *m = new message;
	uint16_t* d = new uint16_t[16];

	for (int i=0; i<16; ++i)
		d[i] = 0x000F;

	m->id = 0;
	strcpy(m->source, "test");
	strcpy(m->dest, s);

	request->op_type = STORE;
	request->address = 0x1111;
	request->data = d;
	m->magic_struct = (void*)request;
	c.onNotify(m);
}

void check_dirty(CacheWritePolicies c, const char *s){
	SAC_to_CWP	*request = new SAC_to_CWP;
	message *m = new message;

	m->id = 0;
	strcpy(m->source, "test");
	strcpy(m->dest, s);

	request->op_type = OpType::CHECK_DIRTY;
	request->address = 0x1111;
	m->magic_struct = (void*)request;
	c.onNotify(m);
}

void load(CacheWritePolicies c, const char *s){
	SAC_to_CWP	*request = new SAC_to_CWP;
	message *m = new message;

	m->id = 0;
	strcpy(m->source, "test");
	strcpy(m->dest, s);

	request->op_type = LOAD;
	request->address = 0x1111;
	m->magic_struct = (void*)request;
	c.onNotify(m);
}

void write(CacheWritePolicies c, const char *s){
	SAC_to_CWP	*request = new SAC_to_CWP;
	message *m = new message;
	uint16_t* d = new uint16_t[16];

	for (int i=0; i<16; ++i)
		d[i] = 0x000F;

	m->id = 0;
	strcpy(m->source, "test");
	strcpy(m->dest, s);
	//reply with PROPAGATE in case of write_through (2, 4)
	//NO_PROPAGATE in case of write_back		    (1, 3)
	d[0] = 0xFFFF;
	request->op_type = WRITE_WITH_POLICIES;
	request->address = 0x1111;
	request->data = d;
	m->magic_struct = (void*)request;
	c.onNotify(m);
}

void invalidate(CacheWritePolicies c, const char *s){
	SAC_to_CWP	*request = new SAC_to_CWP;
	message *m = new message;
	
	m->id = 0;
	strcpy(m->source, "test");
	strcpy(m->dest, s);

	request->op_type = INVALID_LINE;
	request->address = 0x1111;
	m->magic_struct = (void*)request;
	c.onNotify(m);
}
void check_validity(CacheWritePolicies c, const char *s){
	SAC_to_CWP	*request = new SAC_to_CWP;
	message *m = new message;
	
	m->id = 0;
	strcpy(m->source, "test");
	strcpy(m->dest, s);

	request->op_type = CHECK_VALIDITY_DIRTY;
	request->address = 0x1111;
	m->magic_struct = (void*)request;
	c.onNotify(m);
} 
void check_data_validity(CacheWritePolicies c, const char *s){
	SAC_to_CWP	*request = new SAC_to_CWP;
	message *m = new message;
	
	m->id = 0;
	strcpy(m->source, "test");
	strcpy(m->dest, s);

	request->op_type = CHECK_DATA_VALIDITY;
	request->address = 0x1111;
	m->magic_struct = (void*)request;
	c.onNotify(m);
}
/*
This function tests the cache for all the possible requests
*/
void testCache(CacheWritePolicies c, const char *s){
	store(c, s);
	check_dirty(c, s);
	load(c, s);
	write(c, s);
	invalidate(c, s);
	write(c, s);
	check_validity(c, s);
	check_data_validity(c, s);
}

int main(){
	CacheWritePolicies 	cache1("cache1", 1, 4096, 64, WRITE_BACK, WRITE_ALLOCATE),
						cache2("cache2", 1, 4096, 64, WRITE_THROUGH, WRITE_ALLOCATE),
						cache3("cache3", 1, 4096, 64, WRITE_BACK, WRITE_NO_ALLOCATE),
						cache4("cache4", 1, 4096, 64, WRITE_THROUGH, WRITE_NO_ALLOCATE);

	//the tests are repeated for all the possible combinations of cache policies
	testCache(cache1, "cache1");
	testCache(cache2, "cache2");
	testCache(cache3, "cache3");
	testCache(cache4, "cache4");

	return 0;
}
