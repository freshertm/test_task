#include <iostream>
#include <chrono>
#include <boost/asio.hpp>

class repeat_timer
{
public:
    repeat_timer(boost::asio::io_service &io_service, std::chrono::microseconds interval, std::function<void()> handler) : io_service(io_service),
                                                                                                                           timer(io_service, interval),
                                                                                                                           handler_(handler),
                                                                                                                           interval_(interval)
    {
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

        boost::asio::io_service io_service;
        repeat_timer(io_service, std::chrono::seconds(3), &print);

        io_service.run();
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}