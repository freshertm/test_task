#include <iostream>
#include <sstream>
#include <cstdlib>

#include "net_server.h"
#include "common.h"

Response parse(const Request &r)
{
  std::stringstream ss;
  ss << "response: <OK>, received: " << r.size() << " bytes" << std::endl;
  std::string r_string = ss.str();
  Response resp;
  std::copy(r_string.begin(), r_string.end(), std::back_inserter(resp));
  return resp;
}

int main(int argc, char *argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      return 1;
    }

    using namespace std; // For atoi.
    NetServer s(atoi(argv[1]), parse);
    s.serve_forver();
  }
  catch (std::exception &e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}