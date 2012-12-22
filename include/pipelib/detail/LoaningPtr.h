/*
 * LoaningPtr.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef LOANING_PTR_DETAIL_H
#define LOANING_PTR_DETAIL_H

// INCLUDES /////////////////////////////////////////////

namespace pipelib {
namespace detail {

template <typename T>
LoaningPtrBase<T>::LoaningPtrBase(T * const ptr):
myPtr(ptr),
myLoanee(NULL)
{}

template <typename T>
LoaningPtrBase<T>::~LoaningPtrBase()
{
  if(myLoanee)
    myLoanee->recall();
}

template <typename T>
T & LoaningPtrBase<T>::operator* () const
{
  return *myPtr;
}

template <typename T>
T * LoaningPtrBase<T>::operator-> () const
{
  return myPtr.operator ->();
}

template <typename T>
void LoaningPtrBase<T>::reset(T * const ptr)
{
  if(myLoanee)
    myLoanee->recall();
  myPtr.reset(ptr);
}

template <typename T>
typename LoaningPtrBase<T>::LoanPtrType LoaningPtrBase<T>::loan()
{
  return LoanPtrType(*this);
}

template <typename T>
void LoaningPtrBase<T>::returnLoan()
{
  myLoanee = NULL;
  loanReturned();
}

template <typename T>
void LoaningPtrBase<T>::transferLoan(LoanPtrType & newLoanee)
{
  myLoanee = &newLoanee;
}

} // namespace detail

template <typename T, class Notifiee>
LoaningPtr<T, Notifiee>::LoaningPtr(T * const ptr, Notifiee * notifiee):
detail::LoaningPtrBase<T>(ptr),
myNotifiee(notifiee)
{}

template <typename T, class Notifiee>
void LoaningPtr<T, Notifiee>::loanReturned()
{
  if(myNotifiee)
    myNotifiee->loanReturned(*this);
}

template <typename T>
LoanPtr<T>::LoanPtr():
myOwningPtr(NULL)
{}

template <typename T>
LoanPtr<T>::LoanPtr(LoanPtr<T> & rhs):
myOwningPtr(rhs.myOwningPtr)
{
  // Transfer ownership by clearing values in rhs
  rhs.myOwningPtr = NULL;
  if(myOwningPtr)
    myOwningPtr->transferLoan(*this);
}

template <typename T>
LoanPtr<T>::LoanPtr(LoanPtrRef<T> rhs):
myOwningPtr(rhs.ref)
{
  // Transfer ownership by clearing values in rhs
  rhs.ref = NULL;
  if(myOwningPtr)
    myOwningPtr->transferLoan(*this);
}

template <typename T>
LoanPtr<T>::~LoanPtr()
{
  if(myOwningPtr)
    myOwningPtr->returnLoan();
}

template <typename T>
T & LoanPtr<T>::operator* () const
{
  return *myOwningPtr;
}

template <typename T>
T * LoanPtr<T>::operator-> () const
{
  return myOwningPtr->operator->();
}

template <typename T>
LoanPtr<T> &
LoanPtr<T>::operator= (LoanPtr<T> & rhs)
{
  myOwningPtr = rhs.myOwningPtr;
  rhs.myOwningPtr = NULL;

  if(myOwningPtr)
    myOwningPtr->transferLoan(*this);

  return *this;
}

template <typename T>
LoanPtr<T> &
LoanPtr<T>::operator= (LoanPtrRef<T> rhs)
{
  myOwningPtr = rhs.ref;
  rhs.ref = NULL;

  if(myOwningPtr)
    myOwningPtr->transferLoan(*this);

  return *this;
}

template <typename T>
void LoanPtr<T>::reset()
{
  if(myOwningPtr)
    myOwningPtr->returnLoan();
  myOwningPtr = NULL;
}

template <typename T>
LoanPtr<T>::operator LoanPtrRef<T>()
{
  LoaningPtrBase * const tmpPtr = myOwningPtr;
  myOwningPtr = NULL;
  LoanPtrRef<T> ref(myOwningPtr);
  return ref;
}

template <typename T>
LoanPtr<T>::LoanPtr(LoaningPtrBase & owner):
myOwningPtr(&owner)
{}

template <typename T>
void LoanPtr<T>::recall()
{
  myOwningPtr = NULL;
}


}

#endif /* LOANING_PTR_DETAIL_H */
