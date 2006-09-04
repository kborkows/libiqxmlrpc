//  Libiqxmlrpc - an object-oriented XML-RPC solution.
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
//  $Id: conn_factory.h,v 1.2 2006-09-04 12:13:31 adedov Exp $

#ifndef _libiqnet_connection_factory_
#define _libiqnet_connection_factory_

#include <string>
#include "inet_addr.h"

namespace iqnet
{
  class Accepted_conn_factory;

  template<class Conn_type>
  class Serial_conn_factory;
};


//! Abstract factory for accepted connections.
class iqnet::Accepted_conn_factory {
public:
  virtual ~Accepted_conn_factory() {}
  virtual void create_accepted( const Socket& ) = 0;
};


//! Factory class for single threaded connections.
template <class Conn_type>
class iqnet::Serial_conn_factory: public iqnet::Accepted_conn_factory {
public:
  void create_accepted( const Socket& sock )
  {
    Conn_type* c = new Conn_type( sock );
    post_create( c );
    c->post_accept();
  }

  virtual void post_create( Conn_type* ) {}
};

#endif
