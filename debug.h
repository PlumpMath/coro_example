#ifndef _DEBUG_H_
#define _DEBUG_H_
#include <boost/thread.hpp>
#include <iostream>
#include <chrono>

#define YDEBUG 1
#if YDEBUG

# define YLOG(msg) \
  { \
  	std::ostringstream _os_; \
  	_os_ << "[" << boost::this_thread::get_id () << "] "; \
  	_os_ << "[" << __FUNCTION__ << "]: "; \
  	_os_ << msg; \
  	std::cout << _os_.str (); \
  }

# define YLOG_D(msg) YLOG(msg)
#else
# define YLOG(msg) while (0) {}
#endif
#endif // _YDEBUG_H_
