#include "CacheWritePolicies.h"
#include "SetAssociative_WritePolicies_message.h"

#include <string>
#include <vector>
#include <iostream>


CacheWritePolicies::CacheWritePolicies(string name, int priority, uint16_t cache_size, uint16_t line_size, HIT_POLICY hp, MISS_POLICY mp) : module(name, priority), Cache(cache_size, line_size) {
	
	hit_policy = hp;
	miss_policy = mp;
	
}



CacheWritePolicies::onNotify(message *m) {
	
	//Don't forget to check if the message was for me since we are in a broadcast environment
	if(m->dest != getName())
		return;
		
	cout << "Message " << m->id << ": Sent at " << m->timestamp << ", current timestamp " << getTime() << ". My name is " << getName() << "." << endl;
	
	SAC_to_CWP* request_struct = SAC_TO_CWP(m->magic_struct);
	sender_name = string(m->source);
	//allocate the space for the pointer to the response magic struct
	CWP_to_SAC* response_struct;

	
	switch(request_struct->op_type) {
		
		case SET_DIRTY:
			response_struct = set_dirty(request_struct->address, 1);
			break;
		case :
		//...
		
		default:
		
	}
	
	
	// delete the received message (!!!!! CHECK IF IT MUST BE DONE THIS WAY)
	delete m;

	//create the response message
	message response_message = WP_create_message(sender_name);
	response_message->magic_struct = (void*)response_struct;
	
	sendWithDelay(myMessage, CWR_RESPONSE_DELAY);
}


CWP_to_SAC* CacheWritePolicies::WP_set_dirty(SAC_to_CWP* request_struct) {
	
	//check if the block is valid
	bool data_valid = check_data_validity(request_struct->address);
	//perform the operation->set to 1 the dirty bit of the desired block (if it's valid)
	if(data_valid)
		set_dirty(request_struct->address, 1);
	
	//create the reply structure
	CWP_to_SAC* response_struct = new CWP_to_SAC();
	response_struct->hit_flag = data_valid;
	response_struct->data = NULL;
	response_struct->address = request_struct->address;
	
	return response_struct;
}


message* CacheWritePolicies::WP_create_message(string destination) {

	message* myMessage = new message();
	myMessage->valid = 1;
	myMessage->timestamp = getTime();
	strcpy(myMessage->source, getName().c_str());
	strcpy(myMessage->dest, destination.c_str());
	myMessage->magic_struct = NULL;
	
}