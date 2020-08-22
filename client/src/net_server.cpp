//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "net_server.h"

using boost::asio::ip::tcp;

class session
{
public:
  session(boost::asio::io_service &io_service, RequestHandler request_handler)
      : socket_(io_service),
      request_handler_(request_handler)
  {
  }

  tcp::socket &socket()
  {
    return socket_;
  }

  void start()
  {
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
                            boost::bind(&session::handle_read, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
  }

  void handle_read(const boost::system::error_code &error,
                   size_t bytes_transferred)
  {
    if (!error)
    {
      Request r;
      r.assign(data_, data_ + bytes_transferred);
      Response response = request_handler_(r);

      boost::asio::async_write(socket_,
                               boost::asio::buffer(response),
                               boost::bind(&session::handle_write, this,
                                           boost::asio::placeholders::error));
    }
    else
    {
      delete this;
    }
  }

  void handle_write(const boost::system::error_code &error)
  {
    if (!error)
    {
      socket_.async_read_some(boost::asio::buffer(data_, max_length),
                              boost::bind(&session::handle_read, this,
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred));
    }
    else
    {
      delete this;
    }
  }

private:
  RequestHandler request_handler_;
  tcp::socket socket_;
  enum
  {
    max_length = 1024
  };
  char data_[max_length];
};

class NetServer_Private
{
public:
  NetServer_Private(short port, std::function<Response(Request)> handler) : handler_(handler),
                                                                            acceptor_(io_service_, tcp::endpoint(tcp::v4(), port))
  {
    session *new_session = new session(io_service_, handler_);
    acceptor_.async_accept(new_session->socket(),
                           boost::bind(&NetServer_Private::handle_accept, this, new_session,
                                       boost::asio::placeholders::error));
  }

  void handle_accept(session *new_session,
                     const boost::system::error_code &error)
  {
    if (!error)
    {
      new_session->start();
      new_session = new session(io_service_, handler_);
      acceptor_.async_accept(new_session->socket(),
                             boost::bind(&NetServer_Private::handle_accept, this, new_session,
                                         boost::asio::placeholders::error));
    }
    else
    {
      delete new_session;
    }
  }

  void serve_forever()
  {
    io_service_.run();
  }

private:
  boost::asio::io_service io_service_;
  tcp::acceptor acceptor_;
  RequestHandler handler_;
};

NetServer::NetServer(short port, RequestHandler handler) : p_server(new NetServer_Private(port, handler))
{
}
NetServer::~NetServer()
{
  delete p_server;
  p_server = nullptr;
}
void NetServer::serve_forver() { p_server->serve_forever(); }
