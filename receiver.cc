#include "receiver.h"
#include "async_operation.h"

string_future receiver_impl::recv(const std::string& from)
{
    string_promise prom;

    async_operation().work(3,
            [this, prom, from](/*some resault*/) mutable
            {
                prom.set("message from " + from);
            }
    );

    return prom;
}

