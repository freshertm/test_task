#pragma once
#include <boost/asio.hpp>
#include <stdint.h>
#include <string>

struct PingResult
{
    bool success;
    unsigned int ttl;
    unsigned int time;
    std::string destination;

    PingResult(const std::string dest = "") : success(false), ttl(0), time(0), destination(dest){};
    PingResult( unsigned int time,  unsigned int ttl, const std::string &dest) : time(time), ttl(ttl), destination(dest), success(true) {}
};

class Pinger_Private;
class Pinger
{
public:
    Pinger(boost::asio::io_service &svc, std::function<void(const PingResult &)> handler);
    ~Pinger();
    void do_ping(std::string destination);

private:
    Pinger_Private *p_pinger;
};