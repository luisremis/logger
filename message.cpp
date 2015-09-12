#include "message.h"

using namespace std;

int robustRead(int connFd, char * buffer, int length){
    int count = 0, add = 0;
    while(count < length){
        if( (add = read(connFd, buffer, length)) > 0 ){
            count += add;
            buffer += add;
        }
        else if( add < 0 ){
            return -10;
        }
        else if(add == 0){
            break;
        }
    }
    return count;
}

int robustWrite(int connFd, char * buffer, int length){
    int count = 0, add = 0;
    while(count < length){
        if( ( add = write(connFd, buffer, length)) > 0 ){
            count += add;
            buffer += add;
        }
        else if( add < 0 ){
            return -1;
        }
    }
    return count;
}

//write large package on socket
int splitWrite( int connFd, char * data, int size ){
    
    int msg_num = size / BUFFER_MAX + 1;
    int rest = size % ((int)BUFFER_MAX);
    
    //store the should-be message sizes
    message *msgs = new message[msg_num];
    for(int i=1; i < msg_num; i++){
        msgs[i].begin = msgs[i-1].begin + BUFFER_MAX;
    }
    msgs[msg_num-1].length = rest;

    char writeSuccess = 0;
    for(int i=0; i < msg_num; i++){
        //write each message, max size is BUFFER_MAX
        robustWrite( connFd, data + msgs[i].begin, msgs[i].length );
        
        //cout<<"Server: sent " << msgs[i].length << endl;
        //usleep(200*1000);
        
        //make sure client do got the data
        robustRead( connFd, &writeSuccess, 1 );
        
        //cout<<"Server: recv "<<(int)writeSuccess<<endl;
        //usleep(200*1000);

        //if package failed, send again
        if(writeSuccess==0) i--;
    }
    return size;
}

int splitRead( int connFd, char * data, int size ){
    
    int msg_num = size / BUFFER_MAX + 1;
    int rest = size % ((int)BUFFER_MAX);
    
    //store the should-be message sizes
    message *msgs = new message[msg_num];
    for(int i=1; i < msg_num; i++){
        msgs[i].begin = msgs[i-1].begin + BUFFER_MAX;
    }
    msgs[msg_num-1].length = rest;

    char readSuccess = 0;
    int readSize;

    for(int i=0; i < msg_num; i++){

        //read message from socket. max size BUFFER_MAX
        readSize = robustRead( connFd, data + msgs[i].begin, msgs[i].length );
        
        //cout<<"client recv size :" << readSize << endl;
        //usleep(200*1000);

        //check if reading success or not
        if(readSize == msgs[i].length)  readSuccess = 1;
        else    readSuccess = 0;

        //tell server the data transmission status (good or fail)
        robustWrite( connFd, &readSuccess, 1);
        
        //cout<<"Client: sent "<<endl;
        //usleep(200*1000);
        
        //if reading faile, read again
        if(readSuccess==0) i--;
    }
    return size;
}
