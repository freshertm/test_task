#include "interval_timer.h"

interval_timer::interval_timer(boost::asio::io_service &io_service, std::chrono::microseconds interval, std::function<void()> handler) : io_service(io_service),
                                                                                                                                         timer(io_service, interval),
                                                                                                                                         handler_(handler),
                                                                                                                                         interval_(interval)
{
    timer.expires_from_now(std::chrono::milliseconds(1));
    timer.async_wait(std::bind(&interval_timer::handle_timer, this, std::placeholders::_1));
}

void interval_timer::handle_timer(const boost::system::error_code &)
{
    timer.expires_from_now(interval_);
    timer.async_wait(std::bind(&interval_timer::handle_timer, this, std::placeholders::_1));
    handler_();
}