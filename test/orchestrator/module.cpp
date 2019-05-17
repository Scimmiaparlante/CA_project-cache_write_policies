#include <string>
#include <vector>
#include "../../SetAssociative_WritePolicies_message.h"
#include <iostream>


#include "module.h"

int module::_messageId = 0;

module::module(string name, int priority){//Default priority is 0. Higher the value, higher the priority. Negative values are allowed. Modules with higher priority are notified first.
	_name = name;
	_priority = priority;
	_currentTime = 0;
}

string module::getName(){return _name;};

int module::getPriority(){return _priority;}

//This method is used by the system, should not be called directly
vector<event*> module::initialize(){
	vector<event*> result = events;
	events.clear();
	return result;
};

//This method is used by the system, should not be called directly
vector<event*> module::notify(event* e){
	_currentTime = e->time;
	onNotify(e->m);
	vector<event*> result = events;
	events.clear();
	return result;
};

//Returns the current simulated system time
int module::getTime(){
	return _currentTime;
}

//Custom modules must push their messages using this function (delay is optional)
void module::sendWithDelay(message* m, int delay){
	std::cout<<"Response sent by " <<m->source <<" to " <<m->dest <<" with delay " <<delay <<std::endl;
	std::cout<<"Hit flag: " <<((CWP_to_SAC*)(m->magic_struct))->hit_flag <<std::endl;
	std::cout<<"Address: " << std::hex <<((CWP_to_SAC*)(m->magic_struct))->address <<std::endl;
	std::cout<<"Response code: ";

	switch(((CWP_to_SAC*)(m->magic_struct))->wr) {
		case PROPAGATE:
						std::cout<<"PROPAGATE";
						break;
		case NO_PROPAGATE:
						std::cout<<"NO_PROPAGATE";
						break;
		case LOAD_RECALL:
						std::cout<<"LOAD_RECALL";
						break;
		case CHECK_NEXT:
						std::cout<<"CHECK_NEXT";
						break;
		case NOT_NEEDED:
						std::cout<<"NOT_NEEDED";
						break;
	}

	std::cout<<std::endl <<"Data: ";
	if (((CWP_to_SAC*)(m->magic_struct))->data == NULL)
		std::cout<<"NULL";
	else
		for (int i = 0; i < 16; ++i)
			std::cout<< std::hex <<((CWP_to_SAC*)(m->magic_struct))->data[i];

	std::cout<<std::endl <<std::endl <<std::dec;
	
	if (((CWP_to_SAC*)(m->magic_struct))->data != NULL)
		delete ((CWP_to_SAC*)(m->magic_struct))->data;
	delete ((CWP_to_SAC*)(m->magic_struct));
	delete m;
}
