#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <arpa/inet.h>
#include <assert.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

namespace clienTTPP{

    class Client
    {
    public:
        Client();
        Client(int socketFd);
        Client(const Client &src);
        Client &operator=(const Client &src);
        ~Client();

    private:
        int                         _socketFd;
        std::pair<int, std::string> _sendDatas;
        std::pair<int, std::string> _recvDatas;
    };

}


#endif