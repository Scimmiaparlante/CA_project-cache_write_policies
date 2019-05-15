#include<cstring>
#include "CacheWritePolicies.h"

/*
This function tests the cache for all the possible requests
*/
void testCache(CacheWritePolicies c, const char *s){
	SAC_to_CWP	*request = new SAC_to_CWP;
	message *m = new message;
	uint16_t* d = new uint16_t[16];
	
	for (int i=0; i<16; ++i)
		d[i] = 0x0000;
	
	m->id = 0;
	strcpy(m->source, "test");
	strcpy(m->dest, s);
	
	request->op_type = STORE;			
	request->address = 0x1111;
	request->data = d;
	m->magic_struct = (void*)&request;
	c.onNotify(m);
	
	request = new SAC_to_CWP;
	m = new message;
	d = new uint16_t[16];
	for (int i=0; i<16; ++i)
		d[i] = 0x0000;
		
	request->op_type = CHECK_DIRTY;
	request->address = 0x1111;
	m->magic_struct = (void*)&request;
	c.onNotify(m);
	
	request = new SAC_to_CWP;
	m = new message;
	d = new uint16_t[16];
	for (int i=0; i<16; ++i)
		d[i] = 0x0000;
	//reply with 0
	request->op_type = INVALID_LINE;
	request->address = 0x1111;
	m->magic_struct = (void*)&request;
	c.onNotify(m);						
	
	request = new SAC_to_CWP;
	m = new message;
	d = new uint16_t[16];
	for (int i=0; i<16; ++i)
		d[i] = 0x0000;
	request->op_type = LOAD;
	request->address = 0x1111;
	m->magic_struct = (void*)&request;
	c.onNotify(m);
	
	request = new SAC_to_CWP;
	m = new message;
	d = new uint16_t[16];
	for (int i=0; i<16; ++i)
		d[i] = 0x0000;
	//reply with PROPAGATE in case of write_through (2, 4)
	//NO_PROPAGATE in case of write_back		    (1, 3)
	d[0] = 0xFFFF;
	request->op_type = WRITE_WITH_POLICIES;	
	request->address = 0x1111;
	request->data = d;
	m->magic_struct = (void*)&request;
	c.onNotify(m);
	
	request = new SAC_to_CWP;
	m = new message;
	d = new uint16_t[16];
	for (int i=0; i<16; ++i)
		d[i] = 0x0000;
	request->op_type = INVALID_LINE;
	request->address = 0x1111;
	m->magic_struct = (void*)&request;
	c.onNotify(m);
	
	request = new SAC_to_CWP;
	m = new message;
	d = new uint16_t[16];
	for (int i=0; i<16; ++i)
		d[i] = 0x0000;
	//reply with LOAD_RECALL in case of write_allocate (1, 2)
	//CHECK_NEXT in case of write_no_allocate		   (3, 4)
	d[0] = 0xFFFF;
	request->op_type = WRITE_WITH_POLICIES;	
	request->address = 0x1111;
	request->data = d;
	m->magic_struct = (void*)&request;
	c.onNotify(m);
	
	request = new SAC_to_CWP;
	m = new message;
	d = new uint16_t[16];
	for (int i=0; i<16; ++i)
		d[i] = 0x0000;
	request->op_type = CHECK_VALIDITY_DIRTY;
	request->address = 0x1111;
	m->magic_struct = (void*)&request;
	c.onNotify(m);
	
	request = new SAC_to_CWP;
	m = new message;
	d = new uint16_t[16];
	for (int i=0; i<16; ++i)
		d[i] = 0x0000;
	request->op_type = CHECK_DATA_VALIDITY;
	request->address = 0x1111;
	m->magic_struct = (void*)&request;
	c.onNotify(m);
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