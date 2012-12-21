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

namespace detail {

template <typename T>
class OwningPtrBase : private ::boost::noncopyable
{
public:
  typedef LoaningPtr<T> LoaningPtrType;

  explicit OwningPtrBase(T * const ptr);
  virtual ~OwningPtrBase();

  T & operator* () const;
  T * operator-> () const;
  void reset(T * const ptr = 0);
  LoaningPtrType loan();

protected:
  virtual void loanReturned() = 0;

private:
  typedef ::boost::scoped_ptr<T> Ptr;

  void returnLoan();
  void transferLoan(LoaningPtrType & newLoanee);

  const Ptr myPtr;
  LoaningPtrType * myLoanee;

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

template<typename T>
struct LoaningPtrRef
{
  typedef detail::OwningPtrBase<T> OwningPtrBase;
  explicit LoaningPtrRef(OwningPtrBase * rhs): ref(rhs) {}
	OwningPtrBase * ref;
};

template <typename T>
class LoaningPtr
{
  typedef detail::OwningPtrBase<T> OwningPtrBase;
public:
  
  LoaningPtr(LoaningPtr & rhs);
  LoaningPtr(LoaningPtrRef<T> rhs);
  ~LoaningPtr();

  T & operator* () const;
  T * operator-> () const;
  LoaningPtr<T> & operator= (LoaningPtr<T> & rhs);
  LoaningPtr<T> & operator= (LoaningPtrRef<T> & rhs);
  void reset();

  operator LoaningPtrRef<T>();

private:
  explicit LoaningPtr(OwningPtrBase & owner);
  void recall();

  OwningPtrBase * myOwningPtr;

  friend class detail::OwningPtrBase<T>;
};

}

#include "pipelib/detail/NotifyingPtr.h"

#endif /* NOTIFYING_PTR_H */
