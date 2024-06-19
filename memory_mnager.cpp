#include <stdlib.h>
#include <iostream>
#include <string>

//namespace memory manager encapsulating all the management functionalities

namespace MemoryManager
{

//Structure memBlock to represent each block of memory
  struct memBlock
  {
    unsigned int size;
    bool isFree;
  };

//Constants and global variable for book keeping and keeping initial and ending points of the memory block
  #define BLOCK_SIZE sizeof(memBlock)
  memBlock* start;
  unsigned int size;


//Helper functions to calculate total size of contiguous free memory blocks starting from a given block.
  namespace
  {
    unsigned int continuousMemory(memBlock* block)
    {
      memBlock* currBlock = block;

      unsigned int currSize = 0;
    
      while (currBlock->isFree && currSize < size) //Traverses through free memory blocks to calculate the total availability continuous memory.
      {
        currSize += currBlock->size + BLOCK_SIZE; 
        currBlock = &currBlock[currBlock->size + BLOCK_SIZE];
      }


      if (currSize > 0) // Adjusts the size of the initial block to account for the total continuous memory found.
      {
        currSize -= BLOCK_SIZE;
        block->size = currSize;
      }


      return currSize;
    }

    void onOutOfMemory(void)  //If the memory is not availableit calls onOutOfMemory function to handle the situation.
    {
      std::cerr << "Out of memory" << std::endl;
      exit(1);
    }

    memBlock* getNextBlock(memBlock* block) //Returns the next memory block after the current one.
    {
      memBlock* nextBlock = &block[block->size + BLOCK_SIZE];
      return nextBlock >= &start[size] ? NULL : nextBlock;
    }
  }

//Now entering the public block
  void initialize(void* aStart, unsigned int aSize) //Initialize function sets up the memory manager with a starting address and total size.
  {
    start = (memBlock*) aStart;
    start->isFree = true;
    start->size = aSize - BLOCK_SIZE;

    size = aSize;
  } // Marks the entire memory chunk as a single free block initially

  void* allocate(unsigned int aSize) // Allocates a suitable free block 
  {
    memBlock* block = start;

    while (block != NULL)
    {
      unsigned int availableMemory = continuousMemory(block);
      if (availableMemory > aSize) // search for a suitable free block that can accommodate the requested size.
      {
        block->isFree = false;

        if (block->size > aSize + BLOCK_SIZE) // Splits the block if necessary and updates the block's metadata
        {
          memBlock* newBlock = &block[aSize + BLOCK_SIZE];
          if (newBlock < &start[size])
          {
            newBlock->isFree = true;
            newBlock->size = availableMemory - aSize - BLOCK_SIZE;

            block->size = aSize;
          }
        }

        return &block[BLOCK_SIZE]; // returns a pointer t the allocated memory
      } else
      {
        block = getNextBlock(block);
      }
    }

    onOutOfMemory();
  }

  void deallocate(void* addr) // Marks block of memory free.
  {
    
    memBlock* block = &((memBlock*) addr)[-BLOCK_SIZE];
    block->isFree = true;
  }

  unsigned int freeRemaining(void)
  {
    unsigned int freeMemory = 0;

    memBlock* block = start;
    while (block != NULL)
    {
      if (block->isFree)
      {
        freeMemory += block->size;
      }
      block = getNextBlock(block);
    }

    return freeMemory;
  }

  unsigned int smallestFree(void)
  {
    unsigned int smallestBlock = size;

    memBlock* block = start;
    while (block != NULL)
    {
      if (block->isFree && continuousMemory(block) < smallestBlock) {
        smallestBlock = continuousMemory(block);
      }
      block = getNextBlock(block);
    }

    if (smallestBlock == size)
    {
      smallestBlock = 0;
    }

    return smallestBlock;
  }

  unsigned int largestFree(void)
  {
    unsigned int largestBlock = 0;

    memBlock* block = start;
    while (block != NULL)
    {
      if (block->isFree && continuousMemory(block) > largestBlock) {
        largestBlock = continuousMemory(block);
      }
      block = getNextBlock(block);
    }

    return largestBlock;
  }
};


const int MEMORY_MANAGER_SIZE = 65536;
void* managedMemory[MEMORY_MANAGER_SIZE]; 

void report(std::string str) {
  std::cout << str << ": " << std::endl;
  using namespace MemoryManager;
  std::cout << "Free: " << freeRemaining() << std::endl;
  std::cout << "Smallest: " << smallestFree() << std::endl;
  std::cout << "Largest: " << largestFree() << std::endl;

  std::cout << std::endl;
}


//Main functino demonstrates the usage of the memory manager.
int main(void)
{
  using namespace MemoryManager;
  initialize(managedMemory, MEMORY_MANAGER_SIZE); //Initializes the memory manager
  report("Initialized");

  void* allocs[10]; //Allocate memory block
  for (int i = 0; i < 10; i++) {
    allocs[i] = allocate(10);
  }
  report("Allocate 10 x 10");

  void* alloc100 = allocate(100);
  report("Allocate 100");

  void* alloc200 = allocate(200);
  report("Allocate 200");

  for (int i = 0; i < 5; i++) {
    deallocate(allocs[i]); //deallocates memory blocks
  }
  report("Deallocate 10x5");

  deallocate(alloc100);
  report("Deallocate 100");

  deallocate(allocs[8]);
  report("Deallocate 10x1");
}