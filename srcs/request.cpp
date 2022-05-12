#include "../includes/request.hpp"

namespace clienTTPP{

    
    /* Default constructor*/
    Request::Request(): _mapRequestHeaders(), _rawRequest(){

    }

    /* Copy constructor*/
    Request::Request(const Request &src){
        *this = src;
    }

    /* Assign operator*/
    Request &Request::operator=(const Request &src){
        this->_mapRequestHeaders = src._mapRequestHeaders;
        this->_rawRequest = src._rawRequest;
        return (*this);
    }

    /* Destructor*/
    Request::~Request(){
    }

    void Request::addRequestHeader(const std::pair<std::string, std::string> &headerPair){
            _mapRequestHeaders.insert(headerPair);
    }

    void Request::buildRequest(const std::string &method, const std::string &uri){
        std::map<std::string, std::string>::iterator it;

        /* Build headers */
        this->_rawRequest.clear();
        this->_setRequestLine(method, uri);
        it = this->_mapRequestHeaders.begin();
        while (it != this->_mapRequestHeaders.end()){
            this->_rawRequest.append(it->first);
            if (*(it->first.end()) != ' ')
                this->_rawRequest.append(" ");
            this->_rawRequest.append(it->second);
            this->_rawRequest.append("\r\n");
            it++;
        }
        std::cout << "display header: " << _rawRequest << std::endl;
    }

    const std::string &Request::getRawRequest() const{
        return (this->_rawRequest);
    }


    /* Private functions*/

    void Request::_setRequestLine(const std::string &method, const std::string &uri){
        if (method != "POST" && method != "GET"){
            throw requestError();
        }
        this->_rawRequest.append(method).append(" ").append(uri);
        this->_rawRequest.append("HTTP/1.1\r\n");
    }
}