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
class LoanPtr;

namespace detail {

template <typename T>
class LoaningPtrBase : private ::boost::noncopyable
{
public:
  typedef LoanPtr<T> LoanPtrType;

  explicit LoaningPtrBase(T * const ptr);
  virtual ~LoaningPtrBase();

  T & operator* () const;
  T * operator-> () const;
  void reset(T * const ptr = 0);
  LoanPtrType loan();

protected:
  virtual void loanReturned() = 0;

private:
  typedef ::boost::scoped_ptr<T> Ptr;

  void returnLoan();
  void transferLoan(LoanPtrType & newLoanee);

  const Ptr myPtr;
  LoanPtrType * myLoanee;

  friend class pipelib::LoanPtr<T>;
};

} // namespace detail

template <typename T, class Notifiee>
class LoaningPtr : public detail::LoaningPtrBase<T>
{
public:
  explicit LoaningPtr(T * const ptr, Notifiee * notifiee = NULL);
  
protected:
  virtual void loanReturned();

private:
  Notifiee * myNotifiee;
};

template<typename T>
struct LoanPtrRef
{
  typedef detail::LoaningPtrBase<T> LoaningPtrBase;
  explicit LoanPtrRef(LoaningPtrBase * rhs): ref(rhs) {}
	LoaningPtrBase * ref;
};

template <typename T>
class LoanPtr
{
  typedef detail::LoaningPtrBase<T> LoaningPtrBase;
public:
  LoanPtr();
  LoanPtr(LoanPtr & rhs);
  LoanPtr(LoanPtrRef<T> rhs);
  ~LoanPtr();

  T & operator* () const;
  T * operator-> () const;
  LoanPtr<T> & operator= (LoanPtr<T> & rhs);
  LoanPtr<T> & operator= (LoanPtrRef<T> rhs);
  void reset();

  operator LoanPtrRef<T>();

private:
  explicit LoanPtr(LoaningPtrBase & owner);
  void recall();

  LoaningPtrBase * myOwningPtr;

  friend class detail::LoaningPtrBase<T>;
};

}

#include "pipelib/detail/LoaningPtr.h"

#endif /* NOTIFYING_PTR_H */
