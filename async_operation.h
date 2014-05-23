#ifndef ASYNC_SLEEP_H_
#define ASYNC_SLEEP_H_

#include "active_queue.h"
#include "debug.h"

/*
 * Simulate behaviour of async non-blocking operation
 */

class async_operation
{
public:
    typedef boost::asio::deadline_timer timer;
    typedef boost::shared_ptr<timer> timer_ptr;

    template <typename Hook>
    void work(size_t sec, Hook hook)
    {
        timer_ptr t(new timer(active_queue::inst().get_io()));
        t->expires_from_now(boost::posix_time::seconds(sec));
        t->async_wait([t, hook](const boost::system::error_code& error) mutable
        {
            hook();
        });
    }
};



#endif /* ASYNC_SLEEP_H_ */
