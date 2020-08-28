#pragma once

#ifdef USE_ALLOCATOR_MALLOC
#include "allocator_malloc.hh"
template<class T> using RvAllocator = class AllocatorMalloc<T>;
#else
#include <memory>
template<class T> using RvAllocator = class std::allocator<T>;
#endif
