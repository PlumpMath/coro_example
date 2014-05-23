#ifndef RECEIVER_H_
#define RECEIVER_H_


#include "future.h"
#include "module_interface.h"

#include <string>

/*
 * It's true async event driven module
 * It's doesn't block any thread
 *
 * It receive data from 'from'
 */

class receiver: public module_interface
{
public:
  virtual string_future recv (const std::string& from) = 0;
};

class receiver_impl: public receiver
{
public:
  virtual char const* name () const { return "receiver"; }
  virtual string_future recv (const std::string& from);
};


#endif /* RECEIVER_H_ */
