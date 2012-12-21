
#include "RandomStringBlock.h"


RandomStringBlock::RandomStringBlock(const size_t numStrings):
BlockType("Random string block"),
myNumStrings(numStrings)
{}

void RandomStringBlock::start()
{
  for(size_t i = 0; i < myNumStrings; ++i)
  {
    std::string & str = getRunner()->createData();
    str = "random";
    out(str);
  }
}
