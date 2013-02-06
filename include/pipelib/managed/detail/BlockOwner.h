/*
 * BlockOwner.h
 *
 *
 *  Created on: Feb 17, 2012
 *      Author: Martin Uhrin
 */

#ifndef BLOCK_OWNER_H
#define BLOCK_OWNER_H

// INCLUDES /////////////////////////////////////////////
#include "pipelib/Pipeline.h"

#include <set>

#include <boost/foreach.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/type_traits/is_base_of.hpp>

#include "pipelib/BlockIterator.h"
#include "pipelib/managed/detail/BlockOwnerIterator.h"

namespace pipelib {

// FORWARD DECLARATIONS ////////////////////////////////////
template <typename PipelineData, typename SharedData, typename GlobalData>
class Block;
template <typename PipelineData, typename SharedData, typename GlobalData>
class PipeBlock;
template <typename PipelineData, typename SharedData, typename GlobalData>
class StartBlock;

namespace managed {
namespace detail {

template <typename PipelineData, typename SharedData, typename GlobalData>
class BlockOwnerHandle;

template <typename PipelineData, typename SharedData, typename GlobalData>
class BlockOwner;

template <typename PipelineData, typename SharedData, typename GlobalData>
class IBlockOwnerListener
{
public:
  virtual void notifyNoHandles(
    BlockOwner<PipelineData, SharedData, GlobalData> & owner
  ) = 0;
};

template <typename PipelineData, typename SharedData, typename GlobalData>
class BlockOwner : private ::boost::noncopyable
{
  typedef IBlockOwnerListener<PipelineData, SharedData, GlobalData> ListenerType;
  typedef ::std::set<const BlockOwner *> InputBlocks;
public:
  typedef Block<PipelineData, SharedData, GlobalData> BlockType;
  typedef PipeBlock<PipelineData, SharedData, GlobalData> PipeBlockType;
  typedef StartBlock<PipelineData, SharedData, GlobalData> StartBlockType;
  typedef typename UniquePtr<PipeBlockType>::Type PipeBlockPtr;
  typedef typename UniquePtr<StartBlockType>::Type StartBlockPtr;
  typedef BlockOwnerHandle<PipelineData, SharedData, GlobalData> HandleType;
  typedef typename InputBlocks::iterator InputsIterator;
  typedef typename InputBlocks::const_iterator InputsConstIterator;

  //typedef BlockOwnerReverseIterator<BlockOwner, PreorderIncrementer<BlockOwner> > PreorderReverseIterator;
  typedef BlockOwnerReverseIterator<const BlockOwner, PreorderIncrementer<const BlockOwner> > ConstPreorderReverseIterator;

  template <class BlockCreateType>
  static typename UniquePtr<BlockOwner>::Type create(PIPELIB_UNIQUE_PTR(BlockCreateType) blockPtr);

  // TODO: Make constructor arguments UniquePtr<T> types!
  explicit BlockOwner(PipeBlockType * pipeBlock);
  explicit BlockOwner(StartBlockType * startBlock);
  BlockOwner(StartBlockType * startBlock, PipeBlockType * pipeBlock);
  ~BlockOwner();

  ConstPreorderReverseIterator rBeginPreorder() const;
  ConstPreorderReverseIterator rEndPreorder() const;

  BlockType & getBlock();
  const BlockType & getBlock() const;
  PipeBlockType * asPipeBlock();
  const PipeBlockType * asPipeBlock() const;
  StartBlockType * asStartBlock();
  const StartBlockType * asStartBlock() const;

  HandleType createHandle();
  size_t numHandles() const;

  size_t numInputs() const;
  InputsIterator beginInputs();
  InputsConstIterator beginInputs() const;
  InputsIterator endInputs();
  InputsConstIterator endInputs() const;
  ::std::pair<InputsIterator, bool> insertInput(const BlockOwner & input);
  bool eraseInput(const BlockOwner & input);
  void eraseInput(InputsIterator position);

  void setListener(ListenerType * listener);

private:
  typedef ::boost::scoped_ptr<BlockType> BlockScopedPtr;
  typedef ::std::set<HandleType *> Handles;

  void handleCreated(HandleType & handle);
  void handleDestroyed(HandleType & handle);

  BlockScopedPtr myBlock;
  InputBlocks myInputs;
  PipeBlockType * myPipeBlock;
  StartBlockType * myStartBlock;

  Handles myHandles;

  ListenerType * myListener;

  friend class BlockOwnerHandle<PipelineData, SharedData, GlobalData>;
};


template <
  class BlockType,
  typename PipelineData,
  typename SharedData,
  typename GlobalData,
  bool isStartBlockAndPipeBlock
>
struct CreateBlockOwner
{};

template <
  class BlockType,
  typename PipelineData,
  typename SharedData,
  typename GlobalData
>
struct CreateBlockOwner<BlockType, PipelineData, SharedData, GlobalData, false>
{
  typedef BlockOwner<PipelineData, SharedData, GlobalData> BlockOwnerType;
  typedef typename UniquePtr<BlockOwnerType>::Type BlockOwnerPtr;

  static BlockOwnerPtr create(typename UniquePtr<BlockType>::Type blockPtr)
  {
    return BlockOwnerPtr(new BlockOwnerType(blockPtr.release()));
  }
};

template <
  class BlockType,
  typename PipelineData,
  typename SharedData,
  typename GlobalData
>
struct CreateBlockOwner<BlockType, PipelineData, SharedData, GlobalData, true>
{
  typedef BlockOwner<PipelineData, SharedData, GlobalData> BlockOwnerType;
  typedef typename UniquePtr<BlockOwnerType>::Type BlockOwnerPtr;

  static BlockOwnerPtr create(typename UniquePtr<BlockType>::Type blockPtr)
  {
    BlockOwnerType::PipeBlockType * const pipeBlock = blockPtr.get();
    return BlockOwnerPtr(new BlockOwnerType(blockPtr.release(), pipeBlock));
  }
};

template <typename PipelineData, typename SharedData, typename GlobalData>
class BlockOwnerHandle
{
  typedef BlockOwner<PipelineData, SharedData, GlobalData> BlockOwnerType;
public:

  BlockOwnerHandle();
  explicit BlockOwnerHandle(BlockOwnerType & blockOwner);
  BlockOwnerHandle(const BlockOwnerHandle & rhs);
  ~BlockOwnerHandle();

  BlockOwnerHandle & operator =(const BlockOwnerHandle & rhs);

  bool valid() const;

  BlockOwnerType * operator->();
  const BlockOwnerType * operator->() const;

  BlockOwnerType & operator*();
  const BlockOwnerType & operator*() const;

private:
  void invalidate();

  BlockOwnerType * myBlockOwner;

  friend class BlockOwner<PipelineData, SharedData, GlobalData>;
};

template <typename PipelineData, typename SharedData, typename GlobalData>
template <class BlockCreateType>
typename UniquePtr<BlockOwner<PipelineData, SharedData, GlobalData> >::Type
BlockOwner<PipelineData, SharedData, GlobalData>::create(PIPELIB_UNIQUE_PTR(BlockCreateType) blockPtr)
{
  return CreateBlockOwner<
    BlockCreateType,
    PipelineData,
    SharedData,
    GlobalData,
    ::boost::is_base_of<StartBlockType, BlockCreateType>::value && ::boost::is_base_of<PipeBlockType, BlockCreateType>::value
  >::create(blockPtr);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
BlockOwner<PipelineData, SharedData, GlobalData>::BlockOwner(PipeBlockType * pipeBlock):
myPipeBlock(pipeBlock),
myBlock(pipeBlock),
myStartBlock(NULL),
myListener(NULL)
{}

template <typename PipelineData, typename SharedData, typename GlobalData>
BlockOwner<PipelineData, SharedData, GlobalData>::BlockOwner(StartBlockType * startBlock):
myStartBlock(startBlock),
myBlock(startBlock),
myPipeBlock(NULL),
myListener(NULL)
{}

template <typename PipelineData, typename SharedData, typename GlobalData>
BlockOwner<PipelineData, SharedData, GlobalData>::BlockOwner(StartBlockType * startBlock, PipeBlockType * pipeBlock):
myStartBlock(startBlock),
myPipeBlock(pipeBlock),
myBlock(startBlock),
myListener(NULL)
{}

template <typename PipelineData, typename SharedData, typename GlobalData>
BlockOwner<PipelineData, SharedData, GlobalData>::~BlockOwner()
{
  // Invalidate all handles
  BOOST_FOREACH(HandleType * handle, myHandles)
  {
    handle->invalidate();
  }
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename BlockOwner<PipelineData, SharedData, GlobalData>::ConstPreorderReverseIterator
BlockOwner<PipelineData, SharedData, GlobalData>::rBeginPreorder() const
{
  return ConstPreorderReverseIterator(*this);
}


template <typename PipelineData, typename SharedData, typename GlobalData>
typename BlockOwner<PipelineData, SharedData, GlobalData>::ConstPreorderReverseIterator
BlockOwner<PipelineData, SharedData, GlobalData>::rEndPreorder() const
{
  return ConstPreorderReverseIterator();
}


template <typename PipelineData, typename SharedData, typename GlobalData>
typename BlockOwner<PipelineData, SharedData, GlobalData>::BlockType &
BlockOwner<PipelineData, SharedData, GlobalData>::getBlock()
{
  return *myBlock;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
const typename BlockOwner<PipelineData, SharedData, GlobalData>::BlockType &
BlockOwner<PipelineData, SharedData, GlobalData>::getBlock() const
{
  return *myBlock;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename BlockOwner<PipelineData, SharedData, GlobalData>::PipeBlockType *
BlockOwner<PipelineData, SharedData, GlobalData>::asPipeBlock()
{
  return myPipeBlock;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
const typename BlockOwner<PipelineData, SharedData, GlobalData>::PipeBlockType *
BlockOwner<PipelineData, SharedData, GlobalData>::asPipeBlock() const
{
  return myPipeBlock;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename BlockOwner<PipelineData, SharedData, GlobalData>::StartBlockType *
BlockOwner<PipelineData, SharedData, GlobalData>::asStartBlock()
{
  return myStartBlock;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
const typename BlockOwner<PipelineData, SharedData, GlobalData>::StartBlockType *
BlockOwner<PipelineData, SharedData, GlobalData>::asStartBlock() const
{
  return myStartBlock;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename BlockOwner<PipelineData, SharedData, GlobalData>::HandleType
BlockOwner<PipelineData, SharedData, GlobalData>::createHandle()
{
  return HandleType(*this);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
size_t BlockOwner<PipelineData, SharedData, GlobalData>::numHandles() const
{
  return myHandles.size();
}

template <typename PipelineData,typename SharedData, typename GlobalData>
size_t BlockOwner<PipelineData, SharedData, GlobalData>::numInputs() const
{
  return myInputs.size();
}

template <typename PipelineData,typename SharedData, typename GlobalData>
typename BlockOwner<PipelineData, SharedData, GlobalData>::InputsIterator
BlockOwner<PipelineData, SharedData, GlobalData>::beginInputs()
{
  return myInputs.begin();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename BlockOwner<PipelineData, SharedData, GlobalData>::InputsConstIterator
BlockOwner<PipelineData, SharedData, GlobalData>::beginInputs() const
{
  return myInputs.begin();
}

template <typename PipelineData,typename SharedData, typename GlobalData>
typename BlockOwner<PipelineData, SharedData, GlobalData>::InputsIterator
BlockOwner<PipelineData, SharedData, GlobalData>::endInputs()
{
  return myInputs.end();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename BlockOwner<PipelineData, SharedData, GlobalData>::InputsConstIterator
BlockOwner<PipelineData, SharedData, GlobalData>::endInputs() const
{
  return myInputs.end();
}

template <typename PipelineData, typename SharedData, typename GlobalData>
::std::pair<typename BlockOwner<PipelineData, SharedData, GlobalData>::InputsIterator, bool>
BlockOwner<PipelineData, SharedData, GlobalData>::insertInput(const BlockOwner & input)
{
  return myInputs.insert(&input);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
bool BlockOwner<PipelineData, SharedData, GlobalData>::eraseInput(const BlockOwner & input)
{
  const InputBlocks::iterator it = ::std::find(myInputs.begin(), myInputs.end(), &input);
  if(it == myInputs.end())
    return false;

  myInputs.erase(it);
  return true;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void BlockOwner<PipelineData, SharedData, GlobalData>::eraseInput(InputsIterator position)
{
  return myInputs.erase(position);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void BlockOwner<PipelineData, SharedData, GlobalData>::setListener(ListenerType * listener)
{
  myListener = listener;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void BlockOwner<PipelineData, SharedData, GlobalData>::handleCreated(HandleType & handle)
{
  myHandles.insert(&handle);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void BlockOwner<PipelineData, SharedData, GlobalData>::handleDestroyed(HandleType & handle)
{
  Handles::iterator it = myHandles.find(&handle);
  PIPELIB_ASSERT_MSG(it != myHandles.end(), "Cannot find destroyed handle.");
  myHandles.erase(it);

  if(myHandles.empty() && myListener)
    myListener->notifyNoHandles(*this);
}

// BlockOwnerHandle ///////////

template <typename PipelineData, typename SharedData, typename GlobalData>
BlockOwnerHandle<PipelineData, SharedData, GlobalData>::BlockOwnerHandle():
myBlockOwner(NULL)
{}

template <typename PipelineData, typename SharedData, typename GlobalData>
BlockOwnerHandle<PipelineData, SharedData, GlobalData>::BlockOwnerHandle(
  BlockOwnerType & blockOwner):
myBlockOwner(&blockOwner)
{
  if(myBlockOwner)
    myBlockOwner->handleCreated(*this);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
BlockOwnerHandle<PipelineData, SharedData, GlobalData>::BlockOwnerHandle(
  const BlockOwnerHandle & rhs):
myBlockOwner(rhs.myBlockOwner)
{
  if(myBlockOwner)
    myBlockOwner->handleCreated(*this);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
BlockOwnerHandle<PipelineData, SharedData, GlobalData>::~BlockOwnerHandle()
{
  if(myBlockOwner)
    myBlockOwner->handleDestroyed(*this);
}

template <typename PipelineData, typename SharedData, typename GlobalData>
BlockOwnerHandle<PipelineData, SharedData, GlobalData> &
BlockOwnerHandle<PipelineData, SharedData, GlobalData>::operator =(const BlockOwnerHandle & rhs)
{
  if(myBlockOwner == rhs.myBlockOwner)
    return *this;

  if(myBlockOwner)
    myBlockOwner->handleDestroyed(*this);
  myBlockOwner = rhs.myBlockOwner;
  if(myBlockOwner)
    myBlockOwner->handleCreated(*this);

  return *this;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
bool BlockOwnerHandle<PipelineData, SharedData, GlobalData>::valid() const
{
  return myBlockOwner != NULL;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename BlockOwnerHandle<PipelineData, SharedData, GlobalData>::BlockOwnerType *
BlockOwnerHandle<PipelineData, SharedData, GlobalData>::operator->()
{
  return myBlockOwner;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
const typename BlockOwnerHandle<PipelineData, SharedData, GlobalData>::BlockOwnerType *
BlockOwnerHandle<PipelineData, SharedData, GlobalData>::operator->() const
{
  return myBlockOwner;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename BlockOwnerHandle<PipelineData, SharedData, GlobalData>::BlockOwnerType &
BlockOwnerHandle<PipelineData, SharedData, GlobalData>::operator *()
{
  return *myBlockOwner;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
typename const BlockOwnerHandle<PipelineData, SharedData, GlobalData>::BlockOwnerType &
BlockOwnerHandle<PipelineData, SharedData, GlobalData>::operator *() const
{
  return *myBlockOwner;
}

template <typename PipelineData, typename SharedData, typename GlobalData>
void BlockOwnerHandle<PipelineData, SharedData, GlobalData>::invalidate()
{
  myBlockOwner = NULL;
}

} // namespace detail
} // namespace managed
} // namespace pipelib


#endif /* BLOCK_OWNER_H */
