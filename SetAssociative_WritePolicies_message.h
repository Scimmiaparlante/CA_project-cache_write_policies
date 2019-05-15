#ifndef SA_WP_MESSAGES
#define SA_WP_MESSAGES

enum OpType {  	SET_DIRTY,
				CHECK_VALIDITY_DIRTY,
				CHECK_DATA_VALIDITY,
				INVALID_LINE,
				LOAD,
				STORE,
				WRITE_WITH_POLICIES
			};
			
enum WriteResponse {PROPAGATE, NO_PROPAGATE, LOAD, CHECK_NEXT};

struct SAC_to_CWP {
	
	OpType op_type;
	uint16_t address; 
	uint16_t* data;
};


struct CWP_to_SAC {
	
	//read
	bool hit_flag;
	uint16_t* data;
	uint16_t address; 
	//write
	WriteResponse wr;
	
};




#endif //SA_WP_MESSAGES
