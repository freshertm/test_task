#include <iostream>
#include <chrono>
#include <boost/asio.hpp>
#include "pinger.h"

class repeat_timer
{
public:
    repeat_timer(boost::asio::io_service &io_service, std::chrono::microseconds interval, std::function<void()> handler) : io_service(io_service),
                                                                                                                           timer(io_service, interval),
                                                                                                                           handler_(handler),
                                                                                                                           interval_(interval)
    {
        timer.expires_from_now(std::chrono::milliseconds(1));
        timer.async_wait(std::bind(&repeat_timer::handle_timer, this, std::placeholders::_1));
    }

    void handle_timer(const boost::system::error_code &)
    {
        timer.expires_from_now(interval_);
        timer.async_wait(std::bind(&repeat_timer::handle_timer, this, std::placeholders::_1));
        handler_();
    }

private:
    boost::asio::io_service &io_service;
    boost::asio::steady_timer timer;
    std::function<void()> handler_;
    std::chrono::microseconds interval_;
};

void print()
{
    std::cout << "Hello, world!" << std::endl;
}

void ping_handler(const PingResult &pr)
{
    if (!pr.success)
    {
        std::cout << "Ping timeout" <<std::endl;
        return;
    }

    std::cout << "Ping " << pr.destination << " time=" << pr.time << " ms"
              << " ttl=" << pr.ttl << std::endl;
}

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

        boost::asio::io_service io_service;
        Pinger pinger(io_service, &ping_handler);

        repeat_timer timer(io_service, std::chrono::seconds(1),
                     [&pinger, &host] () {
                         std::cout << "pinging ... " << std::endl;
                         pinger.do_ping(host);
                     });


        io_service.run();
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}