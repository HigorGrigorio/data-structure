#include <memory>
#include <type_traits>

namespace collections
{
    namespace __base
    {
        template <typename _Ty, typename _Alloc>
        class Vector_base
        {
        public:
            using allocator_type = _Alloc;
            using alloc_type = typename std::allocator_traits<allocator_type>::template rebind_alloc<_Ty>;
            using pointer = typename std::allocator_traits<alloc_type>::pointer;

        private:
            struct Vector_impl_data
            {
            public:
                pointer _First;
                pointer _Last;
                pointer _End_storage;

                Vector_impl_data() noexcept
                {
                }

                Vector_impl_data(Vector_impl_data &&v) noexcept
                    : _First(v._First),
                      _Last(v._Last),
                      _End_storage(v._End_storage)
                {
                    v._First = v._Last = v._End_storage = pointer();
                }

                void _swap(Vector_impl_data &v) noexcept
                {
                    Vector_impl_data temp;
                    temp._copy(v);
                    v._copy(*this);
                    _copy(temp);
                }

                void _copy(Vector_impl_data const &v) noexcept
                {
                    _First = v._First;
                    _Last = v._Last;
                    _End_storage = v._End_storage;
                }
            };

            struct Vector_impl
                : public alloc_type,
                  public Vector_impl_data
            {
            public:
                Vector_impl() noexcept(std::is_nothrow_default_constructible<alloc_type>::value)
                    : alloc_type()
                {
                }

                Vector_impl(alloc_type &&alloc) noexcept
                    : alloc_type(alloc)
                {
                }

                Vector_impl(alloc_type const &alloc) noexcept
                    : Vector_impl(std::move(alloc))
                {
                }

                Vector_impl(Vector_impl &&v) noexcept
                    : alloc_type(std::move(v)), Vector_impl_data(std::move(v))
                {
                }

                Vector_impl(Vector_impl const &v) noexcept
                    : Vector_impl(std::move(v))
                {
                }

                Vector_impl(alloc_type &&alloc, Vector_impl &&v) noexcept
                    : alloc_type(std::move(alloc)), Vector_impl_data(std::move(v))
                {
                }
            };

            allocator_type &_Get_allocator() noexcept
            {
                return Impl;
            }

            allocator_type const &_Get_allocator() const noexcept
            {
                return Impl;
            }

        public:
            using size_type = size_t;

            allocator_type get_allocator() const noexcept
            {
                return allocator_type(_Get_allocator());
            }

            Vector_base()
            {
            }

            Vector_base(size_type n)
                : Impl()
            {
                _Create_storage(n);
            }

            Vector_base(allocator_type &&alloc)
                : Impl(std::move(alloc))
            {
            }

            Vector_base(size_type n, allocator_type &&alloc)
                : Impl(std::move(alloc))
            {
                _Create_storage(n);
            }

            Vector_base(size_type n, allocator_type const &alloc)
                : Vector_base(n, std::move(alloc))
            {
            }

            Vector_base(Vector_base &&v, allocator_type const &alloc)
                : Impl(std::move(alloc))
            {
                if (alloc == v.get_allocator())
                    this->Impl._swap(v);
                else
                {
                    size_type n = v.Impl._End_storage - v.Impl._First;
                    _Create_storage(n);
                }
            }

        protected:
            Vector_impl Impl;

        private:
            [[nodiscard]] pointer _Allocate(size_type n) const noexcept
            {
                return n ? std::allocator<alloc_type>::allocate(Impl, n) : pointer();
            }

            void _Deallocate(pointer ptr, size_type n) noexcept
            {
                if (ptr)
                    std::allocator<alloc_type>::deallocate(Impl, ptr, n);
            }

            size_type n;

        protected:
            void _Create_storage(size_type n) const noexcept
            {
                this->n = n;
                Impl._First = _Allocate(n);
                Impl._Last = Impl._First;
                Impl._End_storage = Impl._First + n;
            }
        };
    }

    template <typename _Ty, typename _Alloc = std::allocator<_Ty>>
    class vector : public __base::Vector_base<_Ty, _Alloc>
    {
        using _Base = __base::Vector_base<_Ty, _Alloc>;
        using allocator_type = _Base::allocator_type;
        using alloc_traits = std::allocator_traits<allocator_type>;

    public:
        using value_type = _Ty;
        using pointer = _Base::pointer;
        using reference = value_type &;
        using const_reference = const reference;

        vector()
        {
        }

        vector(allocator_type const &alloc)
            : _Base(alloc)
        {
        }
    };
}