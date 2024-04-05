/**
 * \file rv_allocator.hh
 * \brief Provide allocator for use with STL container
 */
#pragma once

#ifdef USE_ALLOCATOR_MALLOC
#include <utils_misc/allocator_malloc.hh>
template<class T> using RvAllocator = class AllocatorMalloc<T>;
#define HAS_ALLOCATOR_REALLOC
#else
#include <memory>
template<class T> using RvAllocator = class std::allocator<T>;
#endif
