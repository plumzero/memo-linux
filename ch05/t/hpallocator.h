
#include <stddef.h>
#include <sys/mman.h>
#include <memory.h>
#include <iostream>

template<typename _Tp>
class hpallocator
{
public:
    typedef size_t     size_type;
    typedef ptrdiff_t  difference_type;
    typedef _Tp*       pointer;
    typedef const _Tp* const_pointer;
    typedef _Tp&       reference;
    typedef const _Tp& const_reference;
    typedef _Tp        value_type;

    template<typename _Tp1>
    struct rebind
    { typedef hpallocator<_Tp1> other; };

    hpallocator() throw() { }

    hpallocator(const hpallocator& __a) throw() { }

    template<typename _Tp1>
    hpallocator(const hpallocator<_Tp1>&) throw() { }

    ~hpallocator() throw() { }

    pointer
    allocate(size_type __n, const void* = 0)
    {
        size_t s = __n * sizeof(_Tp);
        void* p = mmap(nullptr, __n * sizeof(_Tp),
                        PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
        if (p == MAP_FAILED)
            throw(1);
        memset(p, 0, s);

        std::cout << "allocate: " << (void*)p << " size: " << s << std::endl;
        return static_cast<_Tp*>(p);
    }

    void
    deallocate(pointer __p, size_type __s)
    {
        std::cout << "deallocate: " << (void*)__p << " size: " << __s << std::endl;
        if (__p) munmap(__p, __s);
    }

    void 
    construct(pointer __p, const _Tp& __val)
    {
        // std::cout << "construct: " << (void*)__p << " val: " << __val << std::endl;
        ::new((void *)__p) _Tp(__val);
    }

    void 
    destroy(pointer __p)
    {
        // std::cout << "destroy: " << (void*)__p << std::endl;
        __p->~_Tp();
    }

    size_type
    max_size() const noexcept
    { return size_t(-1) / sizeof(_Tp); }

    pointer
    address(reference __x) const noexcept
    {
        // std::cout << "address: " << (void*)&__x << std::endl;
        return reinterpret_cast<_Tp*>
            (&const_cast<char&>(reinterpret_cast<const volatile char&>(__x)));
    }

    const_pointer
    address(const_reference __x) const noexcept
    {
        // std::cout << "address(const): " << (void*)&__x << std::endl;
        return reinterpret_cast<_Tp*>
            (&const_cast<char&>(reinterpret_cast<const volatile char&>(__x)));
    }

    // Inherit everything else.
};