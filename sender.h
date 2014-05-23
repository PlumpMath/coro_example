#ifndef _SENDER_H_
#define _SENDER_H_

#include "future.h"
#include "module_interface.h"

#include <string>

/*
 * It's true async event driven method
 * It's doesn't block any thread
 *
 * It's send data to Mars, so it's take some time
 */

class sender: public module_interface
{
public:
  virtual complete_future send (std::string const& msg) = 0;
};

class sender_impl: public sender
{
public:
  virtual char const* name () const { return "sender"; }
  virtual complete_future send (std::string const& msg);

private:
  mutable boost::mutex mux_;
};

#endif // _Y_SENDER_H_
