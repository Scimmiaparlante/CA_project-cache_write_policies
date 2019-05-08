enum OP_TYPE {READ, WRITE};
enum WRITE_RESPONSE {PROPAGATE, NO_PROPAGATE, LOAD, CHECK_NEXT};

struct SAC_to_CWP {
	
	OP_TYPE op_type;
	uint8_t* address; //I think that...mah
	uint8_t* data;	//I think that...mah
	
};


struct CWP_to_SAC_read {
	
	bool hit_flag;
	uint8_t* data;  //I think that...mah
	
};

struct CWP_to_SAC_write {
	
	WRITE_RESPONSE write_response;
	
};

