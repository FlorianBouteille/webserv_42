#ifndef RESPONSEBUILDER_HPP
#define RESPONSEBUILDER_HPP

#include "WebservIncludes.hpp"

class Response;
class Cgi;

class ResponseBuilder
{
    public:
        static Response build(Request & req, const Config & conf, Client &client, std::vector<Cgi> &cgis);

};

int matchCalc(std::string path, std::string locName);
const Location &getLocation(std::string path, const Config &conf);
void buildErrorPage(Response &resp, const Config conf, Location loc, Request req);

#endif // RESPONSEBUILDER_HPP
