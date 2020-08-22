#pragma once

#include <functional>
#include "common.h"

typedef std::function<Response(const Request&)> RequestHandler;

class NetServer_Private;
class NetServer
{
public:
    NetServer(short port, RequestHandler handler);
    ~NetServer();
    void serve_forver();
private:
    NetServer_Private * p_server;
};
