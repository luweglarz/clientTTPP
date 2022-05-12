#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <iostream>
#include <map>

namespace clienTTPP{

    enum method{
        GET,
        POST,
        UNKNOWN
    };

    class Request
    {
    public:
        Request();
        Request(const std::string &method);
        Request(const Request &src);
        Request &operator=(const Request &src);
        ~Request();

        void setRequestMethod(const method &requestMethod);

    private:
        typedef std::map<std::string, std::string>  mapHeaders;
        typedef std::pair<int, std::string>         rawHeaders;

        mapHeaders  _mapRequestHeaders;
        rawHeaders  _rawRequestHeaders;
        method      _requestMethod;
    };

}

#endif