#ifndef MESSAGE_H
#define MESSAGE_H

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <vector>
#include <stdlib.h>     // atoi
#include <unistd.h>     // sleep
#include <stdio.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mutex>

#include "connections.h"
#include "grep.h"
#include "constant.h"

using namespace std;

class message{
public:
    message(){ begin=0; length=BUFFER_MAX; };
    int begin;
    int length;
};


int robustRead(int connFd, char * buffer, int length);

int robustWrite(int connFd, char * buffer, int length);

//write large package on socket
int splitWrite( int connFd, char * data, int size );

int splitRead( int connFd, char * data, int size );


#endif