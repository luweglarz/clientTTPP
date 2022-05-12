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
#include <exception>
#include "request.hpp"

#define SERVER_PORT 80

namespace clienTTPP{
    
    class Client
    {
    public:
        Client();
        Client(const Client &src);
        Client &operator=(const Client &src);
        ~Client();

        void    connectToServer(const std::string &hostTarget);

        struct socketError : public std::exception{
            const char *what() const throw(){
                return ("Socket error");
            }
        };

    private:
        int                         _socketFd;
        std::pair<int, std::string> _sendDatas;
        std::pair<int, std::string> _recvDatas;

        void    _buildSocket();
    };

}


#endif