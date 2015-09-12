
#C compiler
CC = g++

SRC = log.cpp grep.cpp message.cpp connections.cpp

CC_FLAGS = -pthread -std=c++11

EXE = logger

release:$(SRC)
	$(CC)    $(SRC) $(CC_FLAGS) -o $(EXE) 

testing:$(SRC)
	$(CC)   testingApp.cpp $(CC_FLAGS) -o testingApp 

clean: $(SRC)
	rm -f $(EXE) $(EXE_X) $(EXE).linkinfo 
