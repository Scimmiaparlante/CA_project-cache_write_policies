#include <string>
#include <vector>

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
	std::cout<<"Hit flag: " <<m->magic_struct->hit_flag <<std::endl;
	std::cout<<"Address: " <<m->magic_struct->address <<std::endl;
	std::cout<<"Response code: ";
	
	switch(m->magic_struct->wr) {
		case 0:
				std::cout<<"PROPAGATE";
				break;
		case 1:
				std::cout<<"NO_PROPAGATE";
				break;
		case 2:
				std::cout<<"LOAD_RECALL";
				break;
		case 3:
				std::cout<<"CHECK_NEXT";
				break;
		default:
				break;
	}
	
	std::cout<<std::endl <<"Data: ";
	if (data == NULL)
		std::cout<<"NULL";
	else
		std::cout<<m->magic_struct->data;
	
	std::cout<<std::endl;
}
