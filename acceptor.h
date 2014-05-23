#ifndef ACCEPTOR_H_
#define ACCEPTOR_H_

#include "module_interface.h"

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>


/*
 * It's true async event driven module
 * It's doesn't block any thread
 * It's doesn't return future but register handler
 * React on connect to 127.0.0.1:10000
 */

class acceptor: public module_interface
{
public:
    class handler: public boost::enable_shared_from_this<handler>
    {
    public:
        virtual ~handler(){}

        void operator()(const boost::system::error_code& error,
                boost::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor,
                boost::shared_ptr<boost::asio::ip::tcp::socket> socket);
    protected:
        virtual void work() = 0;
    };

public:
  virtual void subscribe (unsigned short port, boost::shared_ptr<handler>) = 0;
};

class acceptor_impl: public acceptor
{
public:
  virtual char const* name () const { return "acceptor"; }
  virtual void subscribe (unsigned short port, boost::shared_ptr<handler>);

private:

};


#endif /* ACCEPTOR_H_ */
