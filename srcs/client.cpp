#include "../includes/client.hpp"

namespace clienTTPP{

    /* Default constructor*/
    Client::Client(): _socketFd(0), _hostTarget(), _sendDatas(), _recvDatas(),
    _sslCtx(NULL), _sslStruct(NULL){
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
        this->_sslCtx = src._sslCtx;
        this->_sslStruct = src._sslStruct;
        return (*this);
    }

    /* Destructor*/
    Client::~Client(){
        close(this->_socketFd);
        if (this->_sslStruct)
            SSL_shutdown(this->_sslStruct);
        if (this->_sslCtx)
            SSL_free(this->_sslStruct);
    }

    void Client::connectToServer(const std::string scheme, const std::string &hostTarget){
        struct hostent      *hostEnt;
        in_addr_t           hostInAddr;
        struct sockaddr_in  hostSockAddr;
        int                 hostPort;

        _buildSocket();
        if ((hostPort = this->_setHostPort(scheme)) == -1){
            std::cerr << "Wront scheme, cannot connect to the server" << std::endl;
            return ;
        }
        if ((hostEnt = gethostbyname(hostTarget.c_str())) == NULL)
            throw clientError(errno);
        if ((hostInAddr = inet_addr(inet_ntoa(*(struct in_addr*)*(hostEnt->h_addr_list)))) == (in_addr_t)-1)
            throw clientError(errno);
        hostSockAddr.sin_addr.s_addr = hostInAddr;
        hostSockAddr.sin_family = AF_INET;
        hostSockAddr.sin_port = htons(hostPort);
        this->_hostTarget = hostTarget;
        if (connect(this->_socketFd, (struct sockaddr*)&hostSockAddr, sizeof(hostSockAddr)) == -1)
            throw clientError(errno);
        if (hostPort == SERVER_HTTPS_PORT)
            _initConnectSsl();
        else{
            this->_sslCtx = NULL;
            this->_sslStruct = NULL;
        }
    }

    std::string        Client::sendRequest(Request &request, const std::string &method, const std::string &uri){
        size_t      bytesSent = 0;
        size_t      requestSize = 0;
        std::string rawRequestCpy;
        std::thread thread;

        request.addRequestHeader(std::make_pair("Host: ", this->_hostTarget));
        request.addRequestHeader(std::make_pair("User-Agent: ", "clienTTPP"));
        request.addRequestHeader(std::make_pair("Connection: ", "close"));
        request.buildRequest(method, uri);
        requestSize = request.getRawRequest().size();
        rawRequestCpy = request.getRawRequest();
        if (this->_sslStruct)
            thread = std::thread(&Client::_SSL_writeLoop, this, rawRequestCpy);
        else
            thread = std::thread(&Client::_sendLoop, this, rawRequestCpy);
        return (this->_recvRequest(thread));
    }

    /* Private functions */
    void Client::_buildSocket(){
        struct protoent     *protoEnt;

        if ((protoEnt = getprotobyname("tcp")) == NULL)
            throw clientError(errno);
        if ((this->_socketFd = socket(AF_INET, SOCK_STREAM, protoEnt->p_proto)) == -1)
            throw clientError(errno);
    }

    int Client::_setHostPort(const std::string &scheme){
        int hostPort;

        if (scheme == "http")
            return (SERVER_HTTP_PORT);
        else if (scheme == "https")
            return (SERVER_HTTPS_PORT);
        return (-1);
    }

    void    Client::_sendLoop(std::string rawRequest){
        size_t      bytesSent = 0;

        while (rawRequest.size() != 0){
            bytesSent = send(this->_socketFd, rawRequest.c_str(), rawRequest.size(), 0);
            if (bytesSent == -1)
                throw clientError(errno);
            rawRequest.erase(0, bytesSent);
        }
    }

    void    Client::_SSL_writeLoop(std::string rawRequest){
        size_t      bytesSent = 0;

        while (rawRequest.size() != 0){
            bytesSent = SSL_write(this->_sslStruct, rawRequest.c_str(), rawRequest.size());
            if (bytesSent == -1)
                throw clientError(errno);
            rawRequest.erase(0, bytesSent);
        }
    }

    std::string Client::_recvRequest(std::thread &sendThread){
        if (this->_sslStruct)
            return (this->_SSL_readLoop(sendThread));
        return (this->_recvLoop(sendThread));
    }

    std::string    Client::_recvLoop(std::thread &sendThread){
        std::string serverResponse("");
        char        recvBuffer[RECV_BUFFER_SIZE];
        size_t      bytesRecved = 0;

        bzero(recvBuffer, RECV_BUFFER_SIZE);
        while ((bytesRecved = recv(this->_socketFd, recvBuffer, RECV_BUFFER_SIZE, 0)) > 0){
            serverResponse.append(recvBuffer);
            if (bytesRecved == -1)
                throw clientError(errno);
            bzero(recvBuffer, RECV_BUFFER_SIZE);
        }
        sendThread.join();
        return (serverResponse);
    }

    std::string    Client::_SSL_readLoop(std::thread &sendThread){
        std::string serverResponse("");
        char        recvBuffer[RECV_BUFFER_SIZE];
        size_t      bytesRecved = 0;

        bzero(recvBuffer, RECV_BUFFER_SIZE);
        while ((bytesRecved = SSL_read(this->_sslStruct, recvBuffer, RECV_BUFFER_SIZE)) > 0){
            serverResponse.append(recvBuffer);
            if (bytesRecved == -1)
                throw clientError(errno);
            bzero(recvBuffer, RECV_BUFFER_SIZE);
        }
        sendThread.join();
        return (serverResponse);
    }

    void    Client::_initConnectSsl(){
        if (this->_sslCtx == NULL){
            SSL_load_error_strings();
            SSL_library_init();
            this->_sslCtx = SSL_CTX_new(SSLv23_client_method ());
        }

        this->_sslStruct = SSL_new(this->_sslCtx);
        SSL_set_fd(this->_sslStruct, this->_socketFd);
        if ((SSL_connect(this->_sslStruct) < 0)){
            std::cerr << "SSL connection failed" << std::endl;
            return ;
        }
    }

}