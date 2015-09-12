#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <vector>
#include <stdlib.h>     // atoi
#include <unistd.h>     // sleep

#include "connections.h"
#include "grep.h"

using namespace std;

#define NODES_NUMBER 3

std::vector<std::string> address;

void generateLog(int test)
{
    stringstream filename;
    filename << "testLog.log";

    ofstream log(filename.str());

    if (test == 1)
    {
        log << "Log Message A " << machine << std::endl;
        for (int i = 0; i < 10; ++i)
        {
            log << "Log Message " << i << std::endl;
        }
    }
    if (test == 2)
    {
        log << "Log Message A " << machine << std::endl;
        log << "Log Message 1" << std::endl;
        log << "Log Message 2" << std::endl;
        log << "Log different 3: " << machine << std::endl;
        log << "Log Message 4" << std::endl;
        log << "Log Message 5" << std::endl;
    }
    if (test == 3)
    {
        for (unsigned int i = 0; i < 100000000; ++i)
        {
            log << "Log this is a very long message that must be retrieved using a lot of effort " << i << std::endl;
        }
    }

}

std::string generateResults(int test)
{
    std::stringstream output;

    if (test == 1)
    {
        for (int i = 0; i < NODES_NUMBER; ++i)
        {
            output << "Log Message A" << i << std::endl;
            for (int j = 0; j < 10; ++j)
            {
                output << "Log Message " << j << std::endl;
            }
        }

    }
    if (test == 2)
    {
        for (int i = 0; i < NODES_NUMBER; ++i)
        {
            output << "Log Message A" << i << std::endl;
            output << "Log different 3: " << i << std::endl;
        }
    }

    return output.str();
}

void listeningThread(int serverPort)
{
    int ret;
    int connFd = open_socket(serverPort);

    int send = 1;
    ret = read(connFd, &send, sizeof(int));

    int replay = 1;
    ret = write(connFd, &replay, sizeof(int));
}

void connection_thread(std::string address,  int serverPort, int threadId)
{
    int connectionToServer;

    connect_to_server(address.c_str(), serverPort, &connectionToServer);

    int send = 1;
    int ret = write(connectionToServer, &send, sizeof(int) );

    send = 0;
    ret = read (connectionToServer, &send, sizeof(int));

    if (send != 1)
    {
        std::cout << "Error on " << address << std::endl;
    }
}

void listening(int serverPort)
{
    std::string input;

    getline(std::cin, input);
    std::cout << "You entered: " << input << std::endl;

    if (input.compare("exit") == 0)
    {
        std::cout << "Exiting normally " << std::endl;
        exit(0);
    }

    std::vector <std::thread> threads;

    for (int i = 0; i < NODES_NUMBER-1; ++i)
    {
        threads.push_back(std::thread(connection_thread,address.at(i), serverPort, i));
    }

    for (auto& th : threads) th.join();

    std::cout << "All systems responded DONE" << std::endl;
}

void getAdress(std::string filename)
{
    ifstream addFile(filename);

    for (int i = 0; i < NODES_NUMBER -1; ++i)
    {
        std::string str;
        getline(addFile, str);
        address.push_back(str);
        std::cout << "Address " << i << ": " << str << std::endl;
    }
}


int main (int argc, char* argv[])
{
    char a;
    bool flag;

    int serverPort = atoi(argv[1]);
    int machineId = atoi(argv[2]);
    int test = atoi(argv[3]);
    int task;

    if (argc > 4)
    {
        task = atoi(argv[4]); // client or server
    }
    else
    {
        task = 0;
    }

    generateLog(test);

    if (task == false) // task == 0 -> server
    {
        listeningThread(serverPort);
        return 0;
    }
    else if (task == true) // task != o -> client
    {
        getAdress("Address.add");
        listening(serverPort);
        std::cout << "DONE!" << std::endl;
        generateResults(test);
        // Call app
        system("./logger 45454 grep Log testLog.log")
        //check result
    }

    return 0;

}