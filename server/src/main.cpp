#include <iostream>
#include <chrono>
#include <list>

#include <boost/asio.hpp>

#include "pinger.h"
#include "interval_timer.h"

void ping_handler(const PingResult &pr)
{
    if (!pr.success)
    {
        std::cout << "Ping timeout" << std::endl;
        return;
    }

    std::cout << "Ping " << pr.destination << " time=" << pr.time << " ms"
              << " ttl=" << pr.ttl << std::endl;
}

class client
{
public:
    client(Pinger &pinger, const std::string &destination) : destination(destination),
                                                             pinger(pinger)
    {
    }

    void update_status()
    {
        pinger.do_ping(destination, std::bind(&client::ping_handler, this, std::placeholders::_1));
    }

private:
    void ping_handler(const PingResult &pr)
    {
        ::ping_handler(pr);
    }

private:
    std::string destination;
    Pinger &pinger;
};

int main(int argc, char *argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: ping <host>" << std::endl;
#if !defined(BOOST_WINDOWS)
            std::cerr << "(You may need to run this program as root.)" << std::endl;
#endif
            return 1;
        }

        std::string host(argv[1]);

        std::list<client> clients;

        boost::asio::io_service io_service;
        Pinger pinger(io_service);

        clients.push_back(client(pinger, host));

        interval_timer timer(io_service, std::chrono::seconds(1),
                             [&clients]() {
                                 std::cout << "pinging ... " << std::endl;
                                 for (client &cl : clients)
                                 {
                                     cl.update_status();
                                 }
                             });

        io_service.run();
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}