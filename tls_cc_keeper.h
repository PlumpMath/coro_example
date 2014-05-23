#ifndef TLS_CC_KEEPER_H_
#define TLS_CC_KEEPER_H_

#include <memory>
#include <boost/asio/spawn.hpp>

typedef std::shared_ptr<boost::asio::yield_context> yield_context_ptr;
typedef std::shared_ptr<boost::asio::yield_context::callee_type> callee_type_ptr;

struct tls_cc_keeper
{

    static void set(const yield_context_ptr& yield)
    {
        yield_ptr() = yield;
    }

    static yield_context_ptr get()
    {
        return yield_ptr();
    }

    static void reset()
    {
        yield_ptr().reset();
    }

    static bool is_exist()
    {
        return yield_ptr().get() != NULL;
    }

    static yield_context_ptr& yield_ptr()
    {
        thread_local static yield_context_ptr yield;
        return yield;
    }

    static yield_context_ptr make_yield_context(const boost::asio::yield_context& yield)
    {
        return std::make_shared<boost::asio::yield_context>(yield);
    }
};


#endif /* TLS_CC_KEEPER_H_ */
