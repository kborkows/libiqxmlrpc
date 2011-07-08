//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "ssl_lib.h"
#include "net_except.h"

using namespace iqnet::ssl;

iqnet::ssl::Ctx* iqnet::ssl::ctx = 0;
bool Ctx::initialized = false;

Ctx* Ctx::client_server( const std::string& cert_path, const std::string& key_path )
{
  return new Ctx( cert_path, key_path, true );
}


Ctx* Ctx::server_only( const std::string& cert_path, const std::string& key_path )
{
  return new Ctx( cert_path, key_path, false );
}


Ctx* Ctx::client_only()
{
  return new Ctx;
}


Ctx::Ctx( const std::string& cert_path, const std::string& key_path, bool client )
{
  init_library();

  ctx = SSL_CTX_new( client ? SSLv23_method() : SSLv23_server_method() );

  if(
    !SSL_CTX_use_certificate_file( ctx, cert_path.c_str(), SSL_FILETYPE_PEM ) ||
    !SSL_CTX_use_PrivateKey_file( ctx, key_path.c_str(), SSL_FILETYPE_PEM ) ||
    !SSL_CTX_check_private_key( ctx )
  )
    throw exception();
}


Ctx::Ctx()
{
  init_library();

  ctx = SSL_CTX_new( SSLv23_client_method() );
}


Ctx::~Ctx()
{
}


void Ctx::init_library()
{
  if( !Ctx::initialized )
  {
    SSL_load_error_strings();
    SSL_library_init();
    Ctx::initialized = true;
  }
}


// ----------------------------------------------------------------------------
iqnet::ssl::exception::exception() throw():
  ssl_err( ERR_get_error() ),
	  msg( ERR_reason_error_string(ssl_err) )
{
	msg.insert(0,"iqnet::ssl");
}


iqnet::ssl::exception::exception( unsigned long err ) throw():
  ssl_err(err),
  msg( ERR_reason_error_string(ssl_err) )
{
	msg.insert(0,"iqnet::ssl");
}


iqnet::ssl::exception::exception( const std::string& msg_ ) throw():
  ssl_err(0),
  msg( msg_ )
{
	msg.insert(0,"iqnet::ssl");
}


// ----------------------------------------------------------------------------
void iqnet::ssl::throw_io_exception( SSL* ssl, int ret )
{
  int code = SSL_get_error( ssl, ret );
  switch( code )
  {
    case SSL_ERROR_NONE:
      return;

    case SSL_ERROR_ZERO_RETURN:
    {
      bool clean = SSL_get_shutdown(ssl) & SSL_RECEIVED_SHUTDOWN;
      throw connection_close( clean );
    }

    case SSL_ERROR_WANT_READ:
      throw need_read();

    case SSL_ERROR_WANT_WRITE:
      throw need_write();

    case SSL_ERROR_SYSCALL:
      if( !ret )
        throw connection_close( false );
      else
        throw iqnet::network_error( "iqnet::ssl::throw_io_exception" );

    case SSL_ERROR_SSL:
      throw exception();

    default:
      throw io_error( code );
  }
}
