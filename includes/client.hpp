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
#include <openssl/ssl.h>
#include <unistd.h>
#include <exception>
#include "request.hpp"

#define SERVER_HTTP_PORT 80
#define SERVER_HTTPS_PORT 443
#define RECV_BUFFER_SIZE 100000

namespace clienTTPP{
    
    class Client
    {
    public:
        Client();
        Client(const Client &src);
        Client &operator=(const Client &src);
        ~Client();

        void        connectToServer(const std::string scheme, const std::string &hostTarget);
        std::string sendRequest(Request &request, const std::string &method, const std::string &uri);

    class clientError: public std::exception {

    public:
        clientError(int errorCode):
        _errorMessage(strerror(errorCode))
        {}

        virtual ~clientError() throw () {}
        const char* what() const throw () {
            return (_errorMessage.c_str());
        }
    private:
        std::string _errorMessage;

};

    private:
        int                         _socketFd;
        std::string                 _hostTarget;
        std::pair<int, std::string> _sendDatas;
        std::pair<int, std::string> _recvDatas;
        SSL_CTX                     *_sslCtx;
        SSL                         *_sslStruct;

        void        _buildSocket();
        std::string _recvRequest();
        std::string _SSL_readLoop();
        std::string _recvLoop();
        int         _setHostPort(const std::string &scheme);
        void        _initConnectSsl();
    };

}


#endif