//
// ping.cpp
// ~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <istream>
#include <iostream>
#include <ostream>

#include "icmp_header.hpp"
#include "ipv4_header.hpp"
#include "pinger.h"

using boost::asio::deadline_timer;
using boost::asio::ip::icmp;
namespace posix_time = boost::posix_time;

class Pinger_Private
{
public:
  Pinger_Private(boost::asio::io_service &io_service, std::function<void(const PingResult &)> handler)
      : resolver_(io_service), socket_(io_service, icmp::v4()),
        timer_(io_service), sequence_number_(0), num_replies_(0),
        handler_(handler)
  {
  }

  void do_ping(std::string destination)
  {
    icmp::resolver::query query(icmp::v4(), destination, "");
    destination_ = *resolver_.resolve(query);
    is_timeout_ = false;
    start_send();
    start_receive();
  }

private:
  void start_send()
  {
    std::string body("\"Hello!\" from Asio ping.");

    // Create an ICMP header for an echo request.
    icmp_header echo_request;
    echo_request.type(icmp_header::echo_request);
    echo_request.code(0);
    echo_request.identifier(get_identifier());
    echo_request.sequence_number(++sequence_number_);
    compute_checksum(echo_request, body.begin(), body.end());

    // Encode the request packet.
    boost::asio::streambuf request_buffer;
    std::ostream os(&request_buffer);
    os << echo_request << body;

    // Send the request.
    time_sent_ = posix_time::microsec_clock::universal_time();
    socket_.send_to(request_buffer.data(), destination_);

    // Wait up to 900 milliseconds for a reply.
    timer_.expires_at(time_sent_ + posix_time::milliseconds(900));
    timer_.async_wait(boost::bind(&Pinger_Private::handle_timeout, this, _1));
  }

  void handle_timeout(const boost::system::error_code &error)
  {
    if (error) {
     return;
    }
    is_timeout_ = true;
    handler_(PingResult());
  }

  void start_receive()
  {
    // Discard any data already in the buffer.
    reply_buffer_.consume(reply_buffer_.size());

    // Wait for a reply. We prepare the buffer to receive up to 64KB.
    socket_.async_receive(reply_buffer_.prepare(65536),
                          boost::bind(&Pinger_Private::handle_receive, this, _2));
  }

  void handle_receive(std::size_t length)
  {
   
    // The actual number of bytes received is committed to the buffer so that we
    // can extract it using a std::istream object.
    reply_buffer_.commit(length);

    // Decode the reply packet.
    std::istream is(&reply_buffer_);
    ipv4_header ipv4_hdr;
    icmp_header icmp_hdr;
    is >> ipv4_hdr >> icmp_hdr;

    // We can receive all ICMP packets received by the host, so we need to
    // filter out only the echo replies that match the our identifier and
    // expected sequence number.

    if (is_timeout_) {
      return;
    }

    if (is && icmp_hdr.type() == icmp_header::echo_reply && icmp_hdr.identifier() == get_identifier() && icmp_hdr.sequence_number() == sequence_number_)
    {
      timer_.cancel();

      posix_time::ptime now = posix_time::microsec_clock::universal_time();

      handler_(PingResult((now - time_sent_).total_milliseconds(), ipv4_hdr.time_to_live(), ipv4_hdr.source_address().to_string()));
    } else 
    {
      start_receive();
    }
  }

  static unsigned short get_identifier()
  {
#if defined(BOOST_WINDOWS)
    return static_cast<unsigned short>(::GetCurrentProcessId());
#else
    return static_cast<unsigned short>(::getpid());
#endif
  }

  icmp::resolver resolver_;
  icmp::endpoint destination_;
  icmp::socket socket_;
  deadline_timer timer_;
  unsigned short sequence_number_;
  posix_time::ptime time_sent_;
  boost::asio::streambuf reply_buffer_;
  std::size_t num_replies_;
  std::function<void(const PingResult &)> handler_;
  bool is_timeout_;
};

Pinger::Pinger(boost::asio::io_service &svc, std::function<void(const PingResult &)> handler) : p_pinger(new Pinger_Private(svc, handler))
{
}

Pinger::~Pinger() {
  delete p_pinger;
  p_pinger = nullptr;
}

void Pinger::do_ping(std::string destination)
{
  p_pinger->do_ping(destination);
}
