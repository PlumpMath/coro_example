#include "sender.h"
#include "async_operation.h"

complete_future sender_impl::send(std::string const& msg)
{
    complete_promise prom;

    async_operation().work(4,
            [this, prom, msg](/*some resault*/) mutable
            {
                {
                    boost::unique_lock<boost::mutex> lock(mux_);
                    std::cout << "Sender(async): " << msg << std::endl;
                }
                prom.set();
            }
    );

    return prom;
}
