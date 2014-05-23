#ifndef RETRANSMITTER_H_
#define RETRANSMITTER_H_

#include "future.h"
#include "tls_cc_keeper.h"
#include "module_interface.h"

#include <string>
/*
 * It's coroutine module
 * It's doesn't block any thread
 *
 * It receive data from Saturn and send it to Mars
 */

class retransmitter: public module_interface
{
public:
  virtual complete_future retransmit (int uniq_iq) = 0;
};

class retransmitter_impl: public retransmitter
{
public:
  virtual char const* name () const { return "retransmitter"; }
  virtual complete_future retransmit (int uniq_iq);

private:
  virtual void retransmit_impl (yield_context_ptr& yield, int uniq_iq);
};



#endif /* RETRANSMITTER_H_ */
