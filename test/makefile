#makefile for the tests

CC = g++

MAIN = test

CFLAGS = -Wall -g -std=c++11 -DDEBUG
CXXFLAGS = -Wall -g -std=c++11 -DDEBUG

test: CWP_test CacheWritePolicies orchestrator/module.o
	$(CC) -Wall -g -std=c++11 -DDEBUG CWP_test.o orchestrator/module.o CacheWritePolicies.o -o test

CWP_test:	CWP_test.cpp CacheWritePolicies.h
	$(CC) -Wall -g -std=c++11 -DDEBUG -c CWP_test.cpp -o CWP_test.o

CacheWritePolicies: CacheWritePolicies.cpp CacheWritePolicies.h dm_cache.hh SetAssociative_WritePolicies_message.h orchestrator/structures.h orchestrator/module.h
	$(CC) -Wall -g -std=c++11 -DDEBUG -c CacheWritePolicies.cpp -o CacheWritePolicies.o

orchestrator/module.o:  orchestrator/module.cpp
	$(CC) -Wall -g -std=c++11 -DDEBUG -c orchestrator/module.cpp -o orchestrator/module.o

clean:
	rm *.o test orchestrator/module.o
