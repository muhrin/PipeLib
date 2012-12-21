/*
 * NotifyingPtr.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef NOTIFYING_PTR_H
#define NOTIFYING_PTR_H

// INCLUDES /////////////////////////////////////////////
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace pipelib {

template <typename T>
class LoaningPtr;

template<typename T>
struct LoaningPtrRef
{
  explicit LoaningPtrRef(T * rhs): ref(rhs) {}
	T * ref;
};

namespace detail {

template <typename T>
class OwningPtrBase : private ::boost::noncopyable
{
public:
  typedef LoaningPtr<T> LoaningPtr;

  explicit OwningPtrBase(T * const ptr);
  virtual ~OwningPtrBase();

  T & operator* () const;
  T * operator-> () const;
  void reset(T * const ptr = 0);
  LoaningPtr loan();

protected:
  virtual void loanReturned() = 0;

private:
  typedef ::boost::scoped_ptr<T> Ptr;

  void returnLoan();
  void transferLoan(LoaningPtr & newLoanee);

  const Ptr myPtr;
  LoaningPtr * myLoanee;

  friend class pipelib::LoaningPtr<T>;
};

} // namespace detail

template <typename T, class Notifiee>
class OwningPtr : public detail::OwningPtrBase<T>
{
public:
  explicit OwningPtr(T * const ptr, Notifiee * notifiee = NULL);
  
protected:
  virtual void loanReturned();

private:
  Notifiee * myNotifiee;
};

template <typename T>
class LoaningPtr
{
  typedef detail::OwningPtrBase<T> OwningPtrBase;
public:
  
  LoaningPtr(LoaningPtr & rhs);
  LoaningPtr(LoaningPtrRef<T> & rhs);
  ~LoaningPtr();

  T & operator* () const;
  T * operator-> () const;
  LoaningPtr & operator= (LoaningPtr & rhs);
  void reset();

private:
  explicit LoaningPtr(OwningPtrBase & owner);
  void recall();

  OwningPtrBase * myOwningPtr;

  friend class detail::OwningPtrBase<T>;
};

}

#include "pipelib/detail/NotifyingPtr.h"

#endif /* NOTIFYING_PTR_H */
