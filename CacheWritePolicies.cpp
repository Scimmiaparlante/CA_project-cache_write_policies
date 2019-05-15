#include "CacheWritePolicies.h"
#include "SetAssociative_WritePolicies_message.h"

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;


CacheWritePolicies::CacheWritePolicies(string name, int priority, uint16_t cache_size, uint16_t line_size, HIT_POLICY hp, MISS_POLICY mp) : module(name, priority), Cache("dm_cache", cache_size, line_size, priority) {
	hit_policy = hp;
	miss_policy = mp;
}


void CacheWritePolicies::onNotify(message *m) {
	
	//Don't forget to check if the message was for me since we are in a broadcast environment
	if(m->dest != getName())
		return;
		
	cout << "Message " << m->id << ": Sent at " << m->timestamp << ", current timestamp " << getTime() << ". My name is " << getName() << "." << endl;
	
	SAC_to_CWP* request_struct = SAC_TO_CWP(m->magic_struct);
	string sender_name = string(m->source);
	//allocate the space for the pointer to the response magic struct
	CWP_to_SAC* response_struct = NULL;
	
	switch(request_struct->op_type) {
		
		case SET_DIRTY:
			response_struct = WP_set_dirty(request_struct);
			break;
		case CHECK_VALIDITY_DIRTY:
			response_struct = WP_check_validity_dirty(request_struct);
			break;
		case CHECK_DATA_VALIDITY:
			response_struct = WP_check_data_validity(request_struct);
			break;
		case INVALID_LINE:
			response_struct = WP_invalid_line(request_struct);
			break;
		case LOAD:
			response_struct = WP_load(request_struct);
			break;
		case STORE:
			response_struct = WP_store(request_struct);
			break;		
		case WRITE_WITH_POLICIES:
			response_struct = WP_write_with_policies(request_struct);
			break;			
		default:
			cout << "Cache write policies: Bad request type" << endl;
			break;
	}
	
	
	// delete the received message and structure (!!!!! CHECK IF IT MUST BE DONE THIS WAY)
	delete m->magic_struct;
	delete m;

	//create the response message
	message response_message = WP_create_message(sender_name);
	response_message->magic_struct = (void*)response_struct;
	
	sendWithDelay(myMessage, CWR_RESPONSE_DELAY);
}


CWP_to_SAC* CacheWritePolicies::WP_set_dirty(SAC_to_CWP* request_struct) {
	
	//check if the block is valid
	bool data_valid = dm_cache::check_data_validity(request_struct->address);
	//perform the operation->set to 1 the dirty bit of the desired block (if it's valid)
	if(data_valid)
		dm_cache::set_dirty(request_struct->address, 1);
	
	//create the reply structure
	CWP_to_SAC* response_struct = new CWP_to_SAC();
	response_struct->hit_flag = data_valid;
	response_struct->data = NULL;
	response_struct->address = request_struct->address;
	
	return response_struct;
}

CWP_to_SAC* CacheWritePolicies::WP_check_validity_dirty(SAC_to_CWP* request_struct) {
	
	bool data_valid_dirty = dm_cache::check_validity_dirty(request_struct->address);
	
	//create the reply structure
	CWP_to_SAC* response_struct = new CWP_to_SAC();
	response_struct->hit_flag = data_valid_dirty;
	response_struct->data = NULL;
	response_struct->address = request_struct->address;
	
	return response_struct;
}

CWP_to_SAC* CacheWritePolicies::WP_check_data_validity(SAC_to_CWP* request_struct) {
	
	bool data_valid = dm_cache::check_data_validity(request_struct->address);
	
	//create the reply structure
	CWP_to_SAC* response_struct = new CWP_to_SAC();
	response_struct->hit_flag = data_valid;
	response_struct->data = NULL;
	response_struct->address = request_struct->address;
	
	return response_struct;
}

CWP_to_SAC* CacheWritePolicies::WP_check_dirty(SAC_to_CWP* request_struct) {
	
	bool dirty = check_dirty(request_struct->address);
	
	//create the reply structure
	CWP_to_SAC* response_struct = new CWP_to_SAC();
	response_struct->hit_flag = dirty;
	response_struct->data = NULL;
	response_struct->address = request_struct->address;
	
	return response_struct;
}


CWP_to_SAC* CacheWritePolicies::WP_invalid_line(SAC_to_CWP* request_struct) {
	
	bool data_valid = dm_cache::check_data_validity(request_struct->address);
	
	if(data_valid)
		dm_cache::invalid_line(request_struct->address);
	
	//create the reply structure
	CWP_to_SAC* response_struct = new CWP_to_SAC();
	response_struct->hit_flag = data_valid;
	response_struct->data = NULL;
	response_struct->address = request_struct->address;
	
	return response_struct;
}

CWP_to_SAC* CacheWritePolicies::WP_load(SAC_to_CWP* request_struct) {
	
	bool data_valid = dm_cache::check_data_validity(request_struct->address);
	//if the data is valid and it's the same block the caller wants to write on -> HIT
	bool hit = (data_valid && dm_cache::resolve_tag(request_struct->address) == tag);
	
	//create the reply structure
	CWP_to_SAC* response_struct = new CWP_to_SAC();
	response_struct->hit_flag = hit;
	
	if(hit) {
		vector<uint16_t> data_read = dm_cache::load(request_struct->address);
		response_struct->data = new uint16_t[size_line];
		copy(data_read.begin(), data_read.end(), response_struct->data);
	}
	else {
		response_struct->data = NULL;
	}
	response_struct->address = request_struct->address;
	
	return response_struct;
}

CWP_to_SAC* CacheWritePolicies::WP_store(SAC_to_CWP* request_struct) {
	
	bool data_valid = dm_cache::check_data_validity(request_struct->address);
	vector<uint16_t> data_as_vector(request_struct->data, request_struct->data + size_line);
	
	dm_cache::set_dirty(request_struct->address, 0);
	bool res = dm_cache::store(request_struct->address, data_as_vector);
	
	if(res == false)
		cout << "Cache module returned bad dimension error" << endl;	

	//deallocate data array
	delete request_struct->data;
	
	//create the reply structure
	CWP_to_SAC* response_struct = new CWP_to_SAC();
	response_struct->hit_flag = data_valid;
	response_struct->data = NULL;
	response_struct->address = request_struct->address;
	
	return response_struct;
}


CWP_to_SAC* CacheWritePolicies::WP_write_with_policies(SAC_to_CWP* request_struct) {
	
	//create the reply structure
	CWP_to_SAC* response_struct = new CWP_to_SAC();

	bool data_valid = dm_cache::check_data_validity(request_struct->address);
	//if the data is valid and it's the same block the caller wants to write on -> HIT
	bool hit = (data_valid && dm_cache::resolve_tag(request_struct->address) == tag);
	uint16_t tag = dm_cache::get_tag(request_struct->address);
	vector<uint16_t> data_as_vector(request_struct->data, request_struct->data);
	
	if(hit) {
		if(hit_policy == WRITE_BACK)
			response_message->wr = NO_PROPAGATE;
		else if(hit_policy == WRITE_THROUGH)
			response_message->wr = PROPAGATE;
		
		response_struct->address = request_struct->address;
		
		dm_cache::set_dirty(request_struct->address, 1);
		//read the old data and modify the word
		vector<uint16_t> data_read = dm_cache::load(request_struct->address);
		data_read.at(request_struct->address & !(0xffff << offset_size)) = data_as_vector.at(0);
		bool res = dm_cache::store(request_struct->address, data_read);
	
		if(res == false)
			cout << "Cache write policies: cache module returned bad dimension error" << endl;	
	}
	else { 	//MISS
		if(hit_policy == WRITE_ALLOCATE)
			response_message->wr = LOAD;	
		else if(hit_policy == WRITE_NO_ALLOCATE)
			response_message->wr = CHECK_NEXT;
		
		if(!data_valid)
			response_struct->address = request_struct->address;
		else
			response_struct->address = (tag << index_size) | dm_cache::resolve_index(request_struct->address);
	}
	
	//deallocate data array
	delete request_struct->data;
	
	response_struct->hit_flag = hit;
	response_struct->data = NULL;
	
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