#include "acceptor.h"
#include "active_queue.h"

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

using namespace boost::asio::ip;

void acceptor::handler::operator()(const boost::system::error_code& error,
        boost::shared_ptr<tcp::acceptor> acceptor,
        boost::shared_ptr<tcp::socket> socket)
{
    socket->close();
    acceptor->get_io_service().post(
            boost::bind(&handler::work, this->shared_from_this()));

    boost::shared_ptr<tcp::socket> new_socket =
            boost::make_shared<tcp::socket>(
                    acceptor->get_io_service());

    acceptor->async_accept(*new_socket,
            boost::bind(&handler::operator(), shared_from_this(), _1, acceptor, new_socket));
}

void acceptor_impl::subscribe(unsigned short port, boost::shared_ptr<handler> h)
{
    boost::asio::io_service& io = active_queue::inst().get_io();

    boost::shared_ptr<tcp::socket> socket =
                        boost::make_shared<boost::asio::ip::tcp::socket>(io);

    boost::shared_ptr<tcp::acceptor> acceptor(
            boost::make_shared<tcp::acceptor>(io, tcp::endpoint(tcp::v4(), port)));

    acceptor->async_accept(*socket, boost::bind(&handler::operator(), h, _1, acceptor, socket));
}
