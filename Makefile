CC=gcc
#CPPFLAGS= -Wall  -Werror  -O3 -g 
CPPFLAGS= -Wall  -Werror  -g 
LDFLAGS = -lm

SRC=src/node.o src/options.o src/utils.o src/extensions.o src/queues.o src/engine.o src/search.o src/flow_solver.o
TARGET=flow


debug ?= 1 
ifeq ($(debug),1) 
	CPPFLAGS= -Wall -Werror -g
endif


$(TARGET): $(SRC)
	$(CC) $(SRC) $(CPPFLAGS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET) src/*.o
