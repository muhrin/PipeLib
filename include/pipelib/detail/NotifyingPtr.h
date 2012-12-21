/*
 * LoaningPtr.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef NOTIFYING_PTR_DETAIL_H
#define NOTIFYING_PTR_DETAIL_H

// INCLUDES /////////////////////////////////////////////

namespace pipelib {
namespace detail {

template <typename T>
OwningPtrBase<T>::OwningPtrBase(T * const ptr):
myPtr(ptr),
myLoanee(NULL)
{}

template <typename T>
OwningPtrBase<T>::~OwningPtrBase()
{
  if(myLoanee)
    myLoanee->recall();
}

template <typename T>
T & OwningPtrBase<T>::operator* () const
{
  return *myPtr;
}

template <typename T>
T * OwningPtrBase<T>::operator-> () const
{
  return myPtr.operator ->();
}

template <typename T>
void OwningPtrBase<T>::reset(T * const ptr)
{
  if(myLoanee)
    myLoanee->recall();
  myPtr.reset(ptr);
}

template <typename T>
typename OwningPtrBase<T>::LoaningPtrType OwningPtrBase<T>::loan()
{
  return LoaningPtrType(*this);
}

template <typename T>
void OwningPtrBase<T>::returnLoan()
{
  myLoanee = NULL;
  loanReturned();
}

template <typename T>
void OwningPtrBase<T>::transferLoan(LoaningPtrType & newLoanee)
{
  myLoanee = &newLoanee;
}

} // namespace detail

template <typename T, class Notifiee>
OwningPtr<T, Notifiee>::OwningPtr(T * const ptr, Notifiee * notifiee):
detail::OwningPtrBase<T>(ptr),
myNotifiee(notifiee)
{}

template <typename T, class Notifiee>
void OwningPtr<T, Notifiee>::loanReturned()
{
  if(myNotifiee)
    myNotifiee->loanReturned(*this);
}

template <typename T>
LoaningPtr<T>::LoaningPtr(LoaningPtr<T> & rhs):
myOwningPtr(rhs.myOwningPtr)
{
  // Transfer ownership by clearing values in rhs
  rhs.myOwningPtr = NULL;
  if(myOwningPtr)
    myOwningPtr->transferLoan(*this);
}

template <typename T>
LoaningPtr<T>::LoaningPtr(LoaningPtrRef<T> rhs):
myOwningPtr(rhs.ref)
{
  // Transfer ownership by clearing values in rhs
  rhs.ref = NULL;
  if(myOwningPtr)
    myOwningPtr->transferLoan(*this);
}

template <typename T>
LoaningPtr<T>::~LoaningPtr()
{
  if(myOwningPtr)
    myOwningPtr->returnLoan();
}

template <typename T>
T & LoaningPtr<T>::operator* () const
{
  return *myOwningPtr;
}

template <typename T>
T * LoaningPtr<T>::operator-> () const
{
  return myOwningPtr->operator->();
}

template <typename T>
LoaningPtr<T> &
LoaningPtr<T>::operator= (LoaningPtr<T> & rhs)
{
  myOwningPtr = rhs.myOwningPtr;
  rhs.myOwningPtr = NULL;

  if(myOwningPtr)
    myOwningPtr->transferLoan(*this);

  return *this;
}

template <typename T>
LoaningPtr<T> &
LoaningPtr<T>::operator= (LoaningPtrRef<T> & rhs)
{
  myOwningPtr = rhs.ref;
  rhs.ref = NULL;

  if(myOwningPtr)
    myOwningPtr->transferLoan(*this);

  return *this;
}

template <typename T>
void LoaningPtr<T>::reset()
{
  if(myOwningPtr)
    myOwningPtr->returnLoan();
  myOwningPtr = NULL;
}

template <typename T>
LoaningPtr<T>::operator LoaningPtrRef<T>()
{
  OwningPtrBase * const tmpPtr = myOwningPtr;
  myOwningPtr = NULL;
  LoaningPtrRef<T> ref(myOwningPtr);
  return ref;
}

template <typename T>
LoaningPtr<T>::LoaningPtr(OwningPtrBase & owner):
myOwningPtr(&owner)
{}

template <typename T>
void LoaningPtr<T>::recall()
{
  myOwningPtr = NULL;
}





}

#endif /* NOTIFYING_PTR_DETAIL_H */
