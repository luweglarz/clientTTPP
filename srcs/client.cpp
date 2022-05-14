#include "../includes/client.hpp"

namespace clienTTPP{

    /* Default constructor*/
    Client::Client(): _socketFd(0), _hostTarget(), _sendDatas(), _recvDatas(),
    _sslCtx(NULL), _sslStruct(NULL){
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

        request.addRequestHeader(std::make_pair("Host: ", this->_hostTarget));
        request.addRequestHeader(std::make_pair("User-Agent: ", "clienTTPP"));
        if (request.getMapRequestHeader().count("Accept: ") == 0)
            request.addRequestHeader(std::make_pair("Accept: ", "*/*"));
        request.buildRequest(method, uri);
        requestSize = request.getRawRequest().size();
        rawRequestCpy = request.getRawRequest();
    std::cout << "send boucle1: " << rawRequestCpy << std::endl;
        while (rawRequestCpy.size() != 0){
            std::cout << "send boucle: " << rawRequestCpy << std::endl;
            if (this->_sslStruct)
                bytesSent += SSL_write(this->_sslStruct, rawRequestCpy.c_str(), request.getRawRequest().size());
            else
                bytesSent += send(this->_socketFd, rawRequestCpy.c_str(), request.getRawRequest().size(), 0);
            if (bytesSent == -1)
                throw clientError(errno);
            rawRequestCpy.erase(0, bytesSent);
        }
        std::cout << "hors boucle\n";
        return (this->_recvRequest()); 
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


    /* function to fix */
    std::string Client::_recvRequest(){
        char        recvBuffer[RECV_BUFFER_SIZE];
        std::string serverResponse("");
        size_t      bytesRecved = 0;
            
        bzero(recvBuffer, RECV_BUFFER_SIZE);
        while (bytesRecved > 0){
            std::cout << "recv boucle\n";
            if (this->_sslStruct)
                bytesRecved = SSL_read(this->_sslStruct, recvBuffer, RECV_BUFFER_SIZE);
            else
                bytesRecved = recv(this->_socketFd, recvBuffer, RECV_BUFFER_SIZE, 0);
            serverResponse.append(recvBuffer);
            std::cout << "servrep " << serverResponse << std::endl;
            if (bytesRecved == -1)
                throw clientError(errno);
            bzero(recvBuffer, RECV_BUFFER_SIZE);
        }
        std::cout << "hosobucle\n";
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