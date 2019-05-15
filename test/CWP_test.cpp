#include "CacheWritePolicies.h"

int main(){
	CacheWritePolicies cache1("cache1", 1, 4096, 64, WRITE_BACK, WRITE_ALLOCATE);
	
	SAC_to_CWP	request;
	CWP_to_SAC* response;
	message m;
	uint16_t* d = new uint16_t[16];
	
	m.id = 0;
	m.source = "test";
	m.dest = "cache1";
	
	request.op_type = STORE;
	request.address = 0x1111;
	request.data = d;
	m.magic_struct = (void*)request;
	cache1.onNotify(&m);
	
	request.op_type = CHECK_DIRTY;
	request.address = 0x1111;
	m.magic_struct = (void*)request;
	cache1.onNotify(&m);
	
	request.op_type = INVALID_LINE;
	request.address = 0x1111;
	m.magic_struct = (void*)request;
	cache1.onNotify(&m);// mi deve dire 0
	
	request.op_type = LOAD;
	request.address = 0x1111;
	m.magic_struct = (void*)request;
	cache1.onNotify(&m);
	
	d[0] = 0xFFFF;
	request.op_type = WRITE_WITH_POLICIES;
	request.address = 0x1111;
	request.data = d;
	m.magic_struct = (void*)request;
	cache1.onNotify(&m);
	
	request.op_type = CHECK_VALIDITY_DIRTY;
	request.address = 0x1111;
	m.magic_struct = (void*)request;
	cache1.onNotify(&m);
	
	request.op_type = CHECK_DATA_VALIDITY;
	request.address = 0x1111;
	m.magic_struct = (void*)request;
	cache1.onNotify(&m);
	
	return 0;
}