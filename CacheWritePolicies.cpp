#include "CacheWritePolicies.h"

#include <string>
#include <vector>


CacheWritePolicies::CacheWritePolicies(string name, int priority, uint16_t cache_size, uint16_t line_size, HIT_POLICY hp, MISS_POLICY mp) : module(name, priority), Cache(cache_size, line_size) {
	
	hit_policy = hp;
	miss_policy = mp;
	
}



CacheWritePolicies::onNotify(message *m) {
	
	//Don't forget to check if the message was for me since we are in a broadcast environment
	if(m->dest != getName())
		return;
		
	cout << "Message " << m->id << ": Sent at " << m->timestamp << ", current timestamp " << getTime() << ". My name is " << getName() << "." << endl;
	
	switch((some sort of cast)m->magic_struct->type field) {
		
		case :
		case :
		//...
		
		default:
		
	}
	
}
	