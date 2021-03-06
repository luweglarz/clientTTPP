#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <iostream>
#include <map>
#include <openssl/ssl.h>

namespace clienTTPP{

    class Request
    {
    public:

        typedef std::map<std::string, std::string>  mapHeaders;

        Request();
        Request(const Request &src);
        Request &operator=(const Request &src);
        ~Request();

        void addRequestHeader(const std::pair<std::string, std::string> &headerPair);

        void buildRequest(const std::string &method, const std::string &uri);

        struct requestError : public std::exception{
            const char *what() const throw(){
                return ("Request error");
            }
        };

        const std::string   &getRawRequest() const;
        const mapHeaders    &getMapRequestHeader() const;
        
    private:

        mapHeaders  _mapRequestHeaders;
        std::string _rawRequest;

        void _setRequestLine(const std::string &method, const std::string &uri);
    };

}

#endif