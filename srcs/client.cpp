#include "../includes/client.hpp"

namespace clienTTPP{

    /* Default constructor*/
    Client::Client(): _socketFd(0), _hostTarget(), _sendDatas(), _recvDatas(){
        this->_buildSocket();
    }

    /* Copy constructor*/
    Client::Client(const Client &src){
        *this = src;
    }

    /* Assign operator*/
    Client &Client::operator=(const Client &src){
        this->_socketFd = src._socketFd;
        this->_hostTarget = src._hostTarget;
        this->_sendDatas = src._sendDatas;
        this->_recvDatas = src._recvDatas;
        return (*this);
    }

    /* Destructor*/
    Client::~Client(){
        close(this->_socketFd);
    }

    void Client::connectToServer(const std::string &hostTarget){
        struct hostent      *hostEnt;
        in_addr_t           hostInAddr;
        struct sockaddr_in  hostSockAddr;

        if ((hostEnt = gethostbyname(hostTarget.c_str())) == NULL)
            throw socketError();
        if ((hostInAddr = inet_addr(inet_ntoa(*(struct in_addr*)*(hostEnt->h_addr_list)))) == (in_addr_t)-1)
            throw socketError();
        hostSockAddr.sin_addr.s_addr = hostInAddr;
        hostSockAddr.sin_family = AF_INET;
        hostSockAddr.sin_port = htons(SERVER_PORT);
        this->_hostTarget = hostTarget;
        if (connect(this->_socketFd, (struct sockaddr*)&hostSockAddr, sizeof(hostSockAddr)) == -1)
            throw socketError();
    }

    void        Client::sendRequest(Request &request, const std::string &method, const std::string &uri){
        size_t      bytesSent = 0;
        size_t      requestSize = 0;

        //Set default header
        request.addRequestHeader(std::make_pair("Host: ", this->_hostTarget));
        request.addRequestHeader(std::make_pair("User-Agent: ", "clienTTPP"));
        //if (request.getMapRequestHeader().count("Accept: ") == 0)
            request.addRequestHeader(std::make_pair("Accept: ", "*/*"));
        request.buildRequest(method, uri);
        requestSize = request.getRawRequest().size();
        std::string rawRequestCpy(request.getRawRequest());
        while (bytesSent <= requestSize) {
            bytesSent += send(this->_socketFd, rawRequestCpy.c_str(), request.getRawRequest().size(), 0);
            if (bytesSent == -1)
                throw sendError();
            rawRequestCpy.erase(0, bytesSent);
        }        
    }

    std::string Client::recvRequest(){
        char        recvBuffer[RECV_BUFFER_SIZE];
        std::string serverResponse("");
        size_t      bytesRecved = 0;

        bzero(recvBuffer, RECV_BUFFER_SIZE);
        while ((bytesRecved = recv(this->_socketFd, recvBuffer, RECV_BUFFER_SIZE, 0)) > 0){
            serverResponse.append(recvBuffer);
            bzero(recvBuffer, RECV_BUFFER_SIZE);
        }
        return (serverResponse);
    }

    /* Private functions */
    void Client::_buildSocket(){
        struct protoent     *protoEnt;

        if ((protoEnt = getprotobyname("tcp")) == NULL)
            throw socketError();
        if ((this->_socketFd = socket(AF_INET, SOCK_STREAM, protoEnt->p_proto)) == -1)
            throw socketError();
    }
}