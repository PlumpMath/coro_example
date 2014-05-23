#include "repo.h"
#include "active_queue.h"

#include "sender.h"
#include "receiver.h"
#include "retransmitter.h"

#include "tls_cc_keeper.h"

#include "acceptor.h"

#include <memory>
#include <boost/asio/spawn.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

template <typename CoroFunction>
void spawn(CoroFunction coro_function)
{
    boost::asio::spawn(active_queue::inst().get_io(),
        [coro_function](const boost::asio::yield_context& yield) mutable {
            tls_cc_keeper::set(tls_cc_keeper::make_yield_context(yield));
            coro_function();

            /* Перед тем как отпустить поток управлениея, удалим из tls
             * наш контекст
             */
            tls_cc_keeper::reset();
    });
}

class my_handler: public acceptor::handler
{
    // Эта задача выполняется в io_service::run() так решил создатель модуля acceptor

    /* Мы ставим в очередь обработки задачу "корутина" и завершаем обработчик
     * В задаче "корутина" мы пишем в стиле:
     *  1. другие корутины зовем как есть
     *  2. асинхронные методы зовем через wrap_future
     */

    virtual void work()
    {
        if(way_ == CORO)
            work2();
        else
            work3();
    }

    void work2()
    {
        for(int i=0; i<1; i++)
        {
            spawn([](){
                int uniq_iq = 2; //rand();

                MODULE(retransmitter).retransmit(uniq_iq);
                // if you call here an async operation it will wrapped under you
                // MODULE(sender).send("blash");
                // you even don't need call fu.get() if you don't need resault
                MODULE(retransmitter).retransmit(uniq_iq);
            });
        }
    }

    void work3()
    {
        for(int i=0; i<10; i++)
        {
            int uniq_iq = 3; //rand();

            auto fu = MODULE(retransmitter).retransmit(uniq_iq);
            fu.add_callback([uniq_iq](){
                // will be executed as a coro (extract yield from tls)
                MODULE(retransmitter).retransmit(uniq_iq);
            });
        }
    }

public:
    enum WAY {CORO, ASYNC};

    my_handler(WAY way):way_(way){}

private:
    WAY way_;
};

int main ()
{
  repo::instance().insert("sender", std::make_shared<sender_impl>());
  repo::instance().insert("receiver", std::make_shared<receiver_impl>());
  repo::instance().insert("acceptor", std::make_shared<acceptor_impl>());
  repo::instance().insert("retransmitter", std::make_shared<retransmitter_impl>());

  // does not block
  MODULE(acceptor).subscribe(10000, boost::make_shared<my_handler>(my_handler::CORO));
  MODULE(acceptor).subscribe(10001, boost::make_shared<my_handler>(my_handler::ASYNC));

  pause();
  return 0;
}
