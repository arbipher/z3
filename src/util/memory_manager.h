/*++
Copyright (c) 2006 Microsoft Corporation

Module Name:

    memory_manager.h

Abstract:

    Custom memory layer.

Author:

    Nikolaj Bjorner (nbjorner) 2007-07-24

Revision History:

--*/
#pragma once

#include<cstdlib>
#include<memory>
#include<ostream>
#include<iomanip>
#include "util/z3_exception.h"

#ifndef __has_builtin
#define __has_builtin(x) 0
#endif


#ifdef __GNUC__
# if ((__GNUC__ * 100 + __GNUC_MINOR__) >= 409 || __has_builtin(returns_nonnull)) && !defined(__INTEL_COMPILER)
#  define GCC_RET_NON_NULL __attribute__((returns_nonnull))
# else
#  define GCC_RET_NON_NULL
# endif
# define ALLOC_ATTR __attribute__((malloc)) GCC_RET_NON_NULL
#elif defined(_WINDOWS)
# define ALLOC_ATTR __declspec(restrict)
#else
# define ALLOC_ATTR
#endif


class out_of_memory_error : public z3_error {
public:
    out_of_memory_error();
};

class memory {
public:
    static bool is_out_of_memory();
    static void initialize(size_t max_size);
    static void set_high_watermark(size_t watermak);
    static bool above_high_watermark();
    static void set_max_size(size_t max_size);
    static void set_max_alloc_count(size_t max_count);
    static void finalize(bool shutdown = true);
    static void display_max_usage(std::ostream& os);
    static void display_i_max_usage(std::ostream& os);
    static void deallocate(void* p);
    static ALLOC_ATTR void* allocate(size_t s);
    static ALLOC_ATTR void* reallocate(void *p, size_t s);
#if Z3DEBUG
    static void deallocate(char const* file, int line, void* p);
    static ALLOC_ATTR void* allocate(char const* file, int line, char const* obj, size_t s);
#endif
    static unsigned long long get_allocation_size();
    static unsigned long long get_max_used_memory();
    static unsigned long long get_allocation_count();
    static unsigned long long get_max_memory_size();
    // temporary hack to avoid out-of-memory crash in z3.exe
    static void exit_when_out_of_memory(bool flag, char const * msg);
};


#if Z3DEBUG

#define alloc(T,...) new (memory::allocate(__FILE__,__LINE__,#T, sizeof(T))) T(__VA_ARGS__)
#define dealloc(_ptr_) deallocf(__FILE__,__LINE__,_ptr_)

template<typename T>
void deallocf(char const* file, int line, T * ptr) {
    if (ptr == 0) return;
    ptr->~T();
    memory::deallocate(file, line, ptr);
}

#else 

#define alloc(T,...) new (memory::allocate(sizeof(T))) T(__VA_ARGS__)

template<typename T>
void dealloc(T * ptr) {
    if (ptr == nullptr) return;
    ptr->~T();
    memory::deallocate(ptr);
}

#endif

template<typename T>
ALLOC_ATTR T * alloc_vect(unsigned sz);

template<typename T>
T * alloc_vect(unsigned sz) {
    T * r = static_cast<T*>(memory::allocate(sizeof(T) * sz));
    std::uninitialized_default_construct_n(r, sz);
    return r;
}

template<typename T>
void dealloc_vect(T * ptr, unsigned sz) {
    if (ptr == nullptr) return;
    std::destroy_n(ptr, sz);
    memory::deallocate(ptr);
}

#define alloc_svect(T, sz) static_cast<T*>(memory::allocate(sizeof(T) * sz))

template<typename T>
void dealloc_svect(T * ptr) {
    if (ptr == nullptr) return;
    memory::deallocate(ptr);
}

template <typename T>
struct std_allocator {
    using value_type = T;
    // the constructors must be provided according to cpp docs
    std_allocator() = default;
    template <class U> constexpr std_allocator(const std_allocator<U>&) noexcept {}
 

    T* allocate(std::size_t n) {
        return static_cast<T*>(memory::allocate(n * sizeof(T)));
    }

    void deallocate(T* p, std::size_t n) {
        memory::deallocate(p);
    }
};

// the comparison operators must be provided according to cpp docs
template <class T, class U>
bool operator==(const std_allocator<T>&, const std_allocator<U>&) { return true; }
template <class T, class U>
bool operator!=(const std_allocator<T>&, const std_allocator<U>&) { return false; }

struct mem_stat {
};

inline std::ostream & operator<<(std::ostream & out, mem_stat const & m) {
    double mem = static_cast<double>(memory::get_allocation_size())/static_cast<double>(1024*1024);
    return out << std::fixed << std::setprecision(2) << mem;
}



