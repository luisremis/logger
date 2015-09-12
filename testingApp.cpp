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

#define NODES_NUMBER 7

std::vector<std::string> address;

void generateLog(int test)
{

    stringstream filename;
    filename << "testLog.log";

    ofstream log(filename.str());

    if (test == 1)
    {
        log << "Log Message A" << std::endl;
        for (int i = 0; i < 10; ++i)
        {
            log << "Log Message " << i << std::endl;
        }
    }
    if (test == 2)
    {
        log << "Log Message A" << std::endl;
        log << "Log Message 1" << std::endl;
        log << "Log Message 2" << std::endl;
        log << "Log different 3" << std::endl;
        log << "Log different 3" << std::endl;
        log << "Log different 3" << std::endl;
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

    std::cout << "Logs Generated " << std::endl;

}

std::string checkResults(int test)
{
    std::stringstream output;
    std::string aux;
    ifstream check("output.txt");

    std::cout << "Verifying results... " << std::endl;
    if (test == 1)
    {
        std::cout << "Test 1 ... " << std::endl;
        for (int i = 0; i < NODES_NUMBER; ++i)
        {
            output.str("");
            output << "Log Message A";
            getline(check, aux);
            if (aux.compare(output.str()) != 0)
            {
                cout << "Failed!: " << aux << " != " << output.str() << endl;
                exit(0);
            }
            for (int j = 0; j < 10; ++j)
            {
                output.str("");
                output << "Log Message " << j;
                getline(check, aux);
                if (aux.compare(output.str()) != 0)
                {
                    cout << "Failed!: " << aux << " != " << output.str() << endl;
                    exit(0);
                }
            }
        }
        std::cout << "Test 1 PASS!" << std::endl;

    }
    if (test == 2)
    {
        output.str("");
        output << "Log different 3";
        for (int i = 0; i < NODES_NUMBER * 3; ++i)
        {
            getline(check, aux);
            if (aux.compare(output.str()) != 0)
            {
                cout << "Failed!: " << aux << " != " << output.str() << endl;
                exit(0);
            }
        }
        std::cout << "Test 2 PASS!" << std::endl;
    }

    return output.str();
}

void listeningThread(int serverPort)
{
    int ret;
    int listenFd = open_socket(serverPort);
    int connFd   = listen_socket(listenFd);

    int recv = 1;
    ret = read(connFd, &recv, sizeof(int));

    int replay = 1;
    ret = write(connFd, &replay, sizeof(int));

    std::cout << "Reply sent" << std::endl;
}

void connection_thread(std::string address,  int serverPort, int threadId)
{
    int connectionToServer;

    int ret = connect_to_server(address.c_str(), serverPort, &connectionToServer);
    if (ret != 0)
    {
        printf("Client: cannot connect to server: %s \n", address.c_str());
        return;
    }

    int send = 1;
    ret = write(connectionToServer, &send, sizeof(int) );

    send = 0;
    ret = read (connectionToServer, &send, sizeof(int));

    if (send != 1)
    {
        std::cout << "Error on " << address << std::endl;
    }
}

void listening(int serverPort)
{/*
    std::string input;

    getline(std::cin, input);
    std::cout << "You entered: " << input << std::endl;

    if (input.compare("exit") == 0)
    {
        std::cout << "Exiting normally " << std::endl;
        exit(0);
    }
*/
    std::cout << "Connecting to servers ... " << std::endl;
    std::vector <std::thread> threads;

    for (int i = 1; i < NODES_NUMBER; ++i)
    {
        threads.push_back(std::thread(connection_thread,address.at(i), serverPort, i));
    }

    for (auto& th : threads) th.join();

    std::cout << "All systems responded DONE" << std::endl;
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


int main (int argc, char* argv[])
{
    char a;
    bool flag;

    int serverPort = atoi(argv[1]);
    int test = atoi(argv[2]);
    int task;

    if (argc > 3)
    {
        task = atoi(argv[3]); // client or server
    }
    else
    {
        task = 0;
    }

    generateLog(test);

    if (task == false) // task == 0 -> server
    {
        listeningThread(serverPort);
        system("./logger 45453");
        return 0;
    }
    else if (task == true) // task != o -> client
    {
        getAdress("Address.add");
        listening(serverPort);
        std::cout << "DONE!" << std::endl;
        // Call app
        if (test == 1)
        {
            system("./logger 45453 \"grep Log testLog.log\"");
        }
        else if (test == 2)
        {
            system("./logger 45453 \"grep different testLog.log\"");
        }
        
        //check result
        checkResults(test);
    }

    return 0;

}