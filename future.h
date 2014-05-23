#ifndef _FUTURE_H_
#define _FUTURE_H_
#include <xxx/future/future.hpp>
#include <string>

namespace future = xxx::future;

typedef future::future<std::string> string_future;
typedef future::promise<std::string> string_promise;

typedef future::future<void> complete_future;
typedef future::promise<void> complete_promise;

#endif // _FUTURE_H_
