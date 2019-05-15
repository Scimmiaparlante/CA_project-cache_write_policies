#include "CacheWritePolicies.h"

void testCache(CacheWritePolicies c, const char *s){
	SAC_to_CWP	request;
	message m;
	uint16_t* d = new uint16_t[16];
	
	m.id = 0;
	m.source = "test";
	m.dest = s;
	
	request.op_type = STORE;			// deve restituire load o check next
	request.address = 0x1111;
	request.data = d;
	m.magic_struct = (void*)request;
	c.onNotify(&m);
	
	request.op_type = CHECK_DIRTY;
	request.address = 0x1111;
	m.magic_struct = (void*)request;
	c.onNotify(&m);
	
	request.op_type = INVALID_LINE;
	request.address = 0x1111;
	m.magic_struct = (void*)request;
	c.onNotify(&m);						// mi deve dire 0
	
	request.op_type = LOAD;
	request.address = 0x1111;
	m.magic_struct = (void*)request;
	c.onNotify(&m);
	
	request.op_type = INVALID_LINE;
	request.address = 0x1111;
	m.magic_struct = (void*)request;
	c.onNotify(&m);
	
	d[0] = 0xFFFF;
	request.op_type = WRITE_WITH_POLICIES;	// deve restituire propagate o no
	request.address = 0x1111;
	request.data = d;
	m.magic_struct = (void*)request;
	c.onNotify(&m);
	
	request.op_type = CHECK_VALIDITY_DIRTY;
	request.address = 0x1111;
	m.magic_struct = (void*)request;
	c.onNotify(&m);
	
	request.op_type = CHECK_DATA_VALIDITY;
	request.address = 0x1111;
	m.magic_struct = (void*)request;
	c.onNotify(&m);
}

int main(){
	CacheWritePolicies 	cache1("cache1", 1, 4096, 64, WRITE_BACK, WRITE_ALLOCATE),
						cache2("cache2", 1, 4096, 64, WRITE_THROUGH, WRITE_ALLOCATE),
						cache3("cache3", 1, 4096, 64, WRITE_BACK, WRITE_NO_ALLOCATE),
						cache4("cache4", 1, 4096, 64, WRITE_THROUGH, WRITE_NO_ALLOCATE);
	
	testCache(cache1, "cache1");
	testCache(cache2, "cache2");
	testCache(cache3, "cache3");
	testCache(cache4, "cache4");
	
	return 0;
}