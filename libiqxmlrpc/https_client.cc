//  Libiqnet + Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2004 Anton Dedov
//  
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//  
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//  
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
//  
//  $Id: https_client.cc,v 1.3 2004-06-07 09:45:43 adedov Exp $

#include <iostream>
#include "https_client.h"

using namespace iqxmlrpc;
using namespace iqnet;


Https_client_connection::Https_client_connection( const iqnet::Socket& s ):
  Client_connection( s ),
  iqnet::ssl::Reaction_connection( s ),
  resp_packet(0)
{
  iqnet::ssl::Reaction_connection::sock.set_non_blocking( true );
}


http::Packet* Https_client_connection::do_process_session( const std::string& s )
{
  out_str = s;

  do {
    int to = timeout >= 0 ? timeout*1000 : -1;
    if( !reactor.handle_events(to) )
      throw Client_timeout();
  }
  while( !resp_packet );
    
  return resp_packet;
}


void Https_client_connection::connect_succeed()
{
  reg_send( out_str.c_str(), out_str.length() );
}


void Https_client_connection::send_succeed( bool& )
{
  read_buf[0] = 0;
  reg_recv( read_buf, read_buf_sz );
}


void Https_client_connection::recv_succeed( bool&, int req_len, int sz )
{
  std::string s( sz ? std::string(read_buf, sz) : "" );
  resp_packet = read_response( s );
  
  if( !resp_packet )
  {
    read_buf[0] = 0;
    reg_recv( read_buf, read_buf_sz );
  }
}