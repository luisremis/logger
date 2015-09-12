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
#include "message.h"
#include "ChronoCpu.h"

using namespace std;

std::mutex printLogLock;

int SERVER_PORT;

std::vector<std::stringstream> logs(NODES_NUMBER);
std::vector<std::string> address;
std::stringstream toFile;

void printLog(char * result, int threadId)
{
    printf("\n%s\n", result);
    toFile << result;
    return;
}


//Server Thread
void listeningThread(int serverPort)
{   
    //set connection
    int listenFd = open_socket(serverPort);
    while(true)
    {

        int ret;
        
        int connFd = listen_socket(listenFd);

        //recv command from client
        char* buffer = new char[BUFFER_MAX];
        ret = read (connFd, buffer, BUFFER_MAX);
        
        //get temp output filename
        string filename;
        stringstream ss;
        ss<<rand()%100000;
        ss>>filename;
        filename += ".tmp";
        //cout<<"Server: filename is "<<filename<<endl;

        //grepping
        bool grepFinished = false;
        //std::thread systemCmd(grep_system, filename, "grep apple *.log", &grepFinished);
        std::thread systemCmd(grep_system, filename, buffer, &grepFinished);

        //sync status with client
        while( !grepFinished ){
            //cout<<"Server: grepping"<<endl;
            char running[3] = {1, 10, 1};
            ret = robustWrite(connFd, running, 3 );
            usleep( SLEEP_TIME * 1000 );
        }

        //finish grepping
        systemCmd.join();

        /*
        //check file exist
        struct stat st;
        if(stat( filename.c_str(), &st) != 0){
            cout<<"Server: cannot find file "<<filename<<endl;
            exit(0);
        }

        int fd = open(filename.c_str(), O_RDONLY);
        void * filep = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        close( fd );
        */

        FILE* f = fopen(filename.c_str(), "r");
        fseek(f, 0, SEEK_END);
        size_t filesize = ftell(f);

        char *filep = new char[filesize+1];
        rewind(f);
        fread(filep, sizeof(char), filesize, f);
        filep[filesize] =0;
        filesize++;

        buffer[0] = 0;
        buffer[1] = 8;
        buffer[2] = 8;
        ((int*)buffer)[1] = filesize;
        //cout<<"Server: filesize: "<<st.st_size<<endl;

        robustWrite(connFd, buffer, 8);

        robustRead(connFd, buffer, 8);

        splitWrite(connFd, (char*)filep, filesize);
       

        close(connFd);
        delete [] buffer;
        delete [] filep;

        //cleanning tmp file on server
        string sysCmd = "rm ";
        sysCmd += filename;
        system(sysCmd.c_str());

        //printf("Server: The mission was a complete success!\n");
    }

    return;
}

//Client Thread
void connection_thread(std::string input, std::string address, int serverPort, int threadId)
{
    int connectionToServer;

    int ret = connect_to_server(address.c_str(), serverPort, &connectionToServer);
    if (ret != 0)
    {
        printf("Client: cannot connect to server: %s \n", address.c_str());
        return;
    }
    char* buffer = new char[BUFFER_MAX];


    char * cstr = new char [input.length()+1];
    strcpy (cstr, input.c_str());

    //send command
    ret = write(connectionToServer, cstr, input.length()+1 );

    //check status
    struct pollfd isReady;
    isReady.fd = connectionToServer;
    isReady.events = POLLIN;
    while(true){
        int ret = poll( &isReady, 1, MAX_LATENCY + SLEEP_TIME);
        if(ret==0) 
        {
            printf("Client: the server %s is dead \n",address.c_str() );
            delete [] buffer;
            delete [] cstr;
            close(connectionToServer);
            return;
        }
        else{
            read(connectionToServer, buffer, BUFFER_MAX);
            //robustRead(connectionToServer, buffer, BUFFER_MAX);
            if(buffer[0]==1 && buffer[1]==10 && buffer[2]==1){
                //cout<<"Clinet: socket "<< connectionToServer <<" is running"<<endl;
                //usleep( SLEEP_TIME * 1000 );
                continue;
            }
            else
                break;
        }
    }

    int filesize = ((int*)buffer)[1];
    //cout<<"Client: filesize "<<filesize<<endl;

    write(connectionToServer, buffer, 8 );

    //memory buffer method
    char * result = new char[filesize];
    splitRead(connectionToServer, result, filesize );
    
    //erase file descriptor information
    //result[ strlen(result) - 2 ] = 0;
    
    close(connectionToServer);
    delete [] buffer;

    printLogLock.lock();
    printf("Logs from Machine %s, size is %d: \n", address.c_str(), filesize);
    printLog( result, threadId );
    printLogLock.unlock();
    
    delete [] result;

    //printf("Client %d: mission finished\n", threadId);

    return;
}

void getAdress(std::string filename)
{
    ifstream addFile(filename);

    for (int i = 0; i < NODES_NUMBER; ++i)
    {
        std::string str;
        getline(addFile, str);
        address.push_back(str);
        std::cout << "Address " << i << ": " << str << std::endl;
    }
}

void listeningCin(std::string input ="")
{

    while (true)
    {
        bool flag = true;
        if(input.compare("") == 0)
        {
            std::cout << "Type 'grep' if you want to see logs: ";
            getline(std::cin, input);
            std::cout << "You entered: " << input << std::endl;

            if (input.compare("exit") == 0)
            {
                std::cout << "Exiting normally " << std::endl;
                exit(0);
            }
            flag = false;
        }

        getAdress("Address.add");

        std::vector <std::thread> threads;

        printf("Retriving information from remote machines... \n");

        ChronoCpu chrono("timer");
        chrono.tic();

        for (int i = 0; i < NODES_NUMBER; ++i)
        {
            threads.push_back(std::thread(connection_thread, input, address.at(i), SERVER_PORT, i));
        }

        for (auto& th : threads) th.join();

        /* Your algorithm here */

        chrono.tac();

        printf("Client: mission completed in only %f s \n", (double)chrono.getElapsedStats().lastTime_ms/1000);

        ofstream output("output.txt");
        output << toFile.str();
        //cout << "To be written in the file: " << std::endl;
        //cout << toFile.str();
        output.close();

        if (flag)
        {
            exit(0);
        }
        input = "";
    }

    return;
}


int main (int argc, char* argv[])
{
    char a;
    bool flag;

    SERVER_PORT = atoi(argv[1]);

    string input = "";
    if(argc > 2)
    {
        input.append(argv[2]);
    }

    std::cout << std::endl << "CS425 - MP1: Distributed Logging init." << std::endl;

    std::thread listeningServer(listeningThread, SERVER_PORT);
    usleep(700);

    std::thread cinListening( listeningCin, input);

    cinListening.join();
    listeningServer.join();
    
    return 0;
}
