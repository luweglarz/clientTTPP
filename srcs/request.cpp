#include "../includes/request.hpp"

namespace clienTTPP{

    
    /* Default constructor*/
    Request::Request(): _mapRequestHeaders(), _rawRequestHeaders(), _requestMethod(){

    }

    /* Method constructor*/
    Request::Request(const std::string &method){
        if (method == "POST")
            _requestMethod = POST;
        else if (method == "GET")
            _requestMethod = GET;
        else
            _requestMethod = UNKNOWN;
    }

    /* Copy constructor*/
    Request::Request(const Request &src){
        *this = src;
    }

    /* Assign operator*/
    Request &Request::operator=(const Request &src){
        this->_mapRequestHeaders = src._mapRequestHeaders;
        this->_rawRequestHeaders = src._rawRequestHeaders;
        return (*this);
    }

    /* Destructor*/
    Request::~Request(){
    }

    void    Request::setRequestMethod(const method &requestMethod){
        this->_requestMethod = requestMethod;
    }
}