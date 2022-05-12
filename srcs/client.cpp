#include "../includes/client.hpp"

namespace clienTTPP{

    /* Default constructor*/
    Client::Client(): _socketFd(0), _sendDatas(), _recvDatas() {
    }
    
    /* Socket building constructor*/
    Client::Client(int socketFd):  
    _socketFd(socketFd), 
    _sendDatas(), 
    _recvDatas() {
        
    }

    /* Copy constructor*/
    Client::Client(const Client &src){
        *this = src;
    }

    /* Assign operator*/
    Client &Client::operator=(const Client &src){
        this->_socketFd = src._socketFd;
        this->_sendDatas = src._sendDatas;
        this->_recvDatas = src._recvDatas;
        return (*this);
    }

    /* Destructor*/
    Client::~Client(){
    }
}