#ifndef _libiqnet_connector_h_
#define _libiqnet_connector_h_

#include <string>
#include <libiqnet/inet_addr.h>


namespace iqnet 
{
  class Connector_base;
  template <class Conn_type> class Connector;
};


class iqnet::Connector_base {
protected:
  Inet_addr peer_addr;
  
public:
  Connector_base( const iqnet::Inet_addr& addr ): 
    peer_addr(addr) {}
      
  virtual ~Connector_base() {}

protected:
  int socket_connect();
};


template <class Conn_type>
class iqnet::Connector: public iqnet::Connector_base {
public:
  Connector( const iqnet::Inet_addr& peer ):
    Connector_base( peer ) {}

  Conn_type* connect()
  {
    int sock = socket_connect();
    Conn_type* c = new Conn_type( sock, peer_addr );
    c->post_connect();
    return c;
  }
};


#endif
