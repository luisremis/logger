#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "connections.h"


int open_socket(int port)
{
    struct sockaddr_in svrAdd, clntAdd;
    int listenFd;
    
    //create socket
    listenFd = socket(AF_INET, SOCK_STREAM, 0);
    int yes = 1;

    if (setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) 
    {
        printf("setsockopt\n");
        exit(1);
    }
    
    if(listenFd < 0)
    {
        std::cout << "Cannot open socket" << std::endl;
        exit(1);
    }
    
    memset((char*) &svrAdd,0, sizeof(svrAdd));
    
    svrAdd.sin_family = AF_INET;
    svrAdd.sin_addr.s_addr = INADDR_ANY;
    svrAdd.sin_port = htons(port);
    
    //bind socket
    if(bind(listenFd, (struct sockaddr *)&svrAdd, sizeof(svrAdd)) < 0)
    {
        std::cout << "open_socket: Cannot bind" << std::endl;
        exit(1);
    }
    printf("Listening at port %d...\n", port);

    return listenFd;
}

int listen_socket(int listenFd)
{

    socklen_t len; //store size of the address
    int connFd;
    struct sockaddr_in svrAdd, clntAdd;

    listen(listenFd, 5);
    
    len = sizeof(clntAdd);

    //this is where client connects. svr will hang in this mode until client conn
    connFd = accept(listenFd, (struct sockaddr *)&clntAdd, &len);

    if (connFd < 0)
    {
        std::cout << "Cannot accept connection" << std::endl;
        exit(1);
    }
    else
    {
        //std::cout << "Server Connection successful " << std::endl;
    }

    return connFd;
}

/*Client operation */
int connect_to_server(const char* add, int port, int* connectionFd)
{
    struct sockaddr_in svrAdd;
    struct hostent *server;
    
    if((port > 65535) || (port < 2000))
    {
        std::cout<<"Please enter port number between 2000 - 65535"<<std::endl;
        exit(1);
    }       
    
    //create client skt
    *connectionFd = socket(AF_INET, SOCK_STREAM, 0);
        int iSetOption = 1;
    setsockopt(*connectionFd, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption,
        sizeof(iSetOption));
    
    if(*connectionFd < 0)
    {
        std::cout << "Cannot open socket" << std::endl;
        exit(1);
    }
    else{
        //std::cout << "Socket opened" << std::endl;
    }
    
    server = gethostbyname(add);
    
    if(server == NULL)
    {
        std::cout << "Host does not exist" << std::endl;
        exit(1);
    }
    
    memset((char *) &svrAdd,0, sizeof(svrAdd));
    svrAdd.sin_family = AF_INET;
    
    memcpy((char *) &svrAdd.sin_addr.s_addr,(char *) server -> h_addr, server -> h_length);
    
    svrAdd.sin_port = htons(port);
    
    int checker = connect(*connectionFd,(struct sockaddr *) &svrAdd, sizeof(svrAdd));
    
    if (checker < 0)
    {
        //printf("Cannot connect to: %s \n",add);
        return -1;
        //exit(1);
    }
    else{
        return 0;
        //std::cout << "Client Connection Successful" << std::endl;
    }
}
