#pragma once
#include <boost/asio.hpp>

class interval_timer
{
public:
    interval_timer(boost::asio::io_service &io_service, std::chrono::microseconds interval, std::function<void()> handler) ;

private:
    void handle_timer(const boost::system::error_code &);

private:
    boost::asio::io_service &io_service;
    boost::asio::steady_timer timer;
    std::function<void()> handler_;
    std::chrono::microseconds interval_;
};