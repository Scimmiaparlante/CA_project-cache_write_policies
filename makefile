MAIN = CacheWritePolicies
CC = g++
CFLAGS = -Wall -std=c++11


#------------------------------------------------------------------------------
#	Dependecies
#------------------------------------------------------------------------------
$(MAIN).o: $(MAIN).cpp
	$(CC) -c $(CFLAGS) $(MAIN).cpp


#-------------------------------------------------------------------------------
# make clean
#-------------------------------------------------------------------------------
clean:
	rm *.o
