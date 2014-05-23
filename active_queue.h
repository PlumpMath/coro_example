#ifndef _ACTIVE_QUEUE_H_
#define _ACTIVE_QUEUE_H_

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "debug.h"

class active_queue
{
public:
    static active_queue& inst()
    {
        static active_queue this_;
        return this_;
    }

    boost::asio::io_service& get_io()
    { return io_; }

    ~active_queue()
    {
        work_.reset();
        workers_.join_all();
    }

private:
    active_queue():work_(new boost::asio::io_service::work(io_))
    {
        workers_.create_thread(boost::bind(&active_queue::run, this));
        workers_.create_thread(boost::bind(&active_queue::run, this));
    }

    void run()
    {
        //YLOG("active_queue started\n");
        io_.run();
        //YLOG("active_queue finished\n");
    }

private:
    boost::thread_group workers_;
    boost::asio::io_service io_;
    std::auto_ptr<boost::asio::io_service::work> work_;
};

#endif // _YP_ACTIVE_QUEUE_H_
