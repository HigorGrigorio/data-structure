#include <memory>
#include <type_traits>
#include <iterator>
#include <bits/allocator.h>

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
                pointer _Start;
                pointer _Last;
                pointer _End_storage;

                Vector_impl_data() noexcept
                {
                }

                Vector_impl_data(Vector_impl_data &&v) noexcept
                    : _Start(v._Start),
                      _Last(v._Last),
                      _End_storage(v._End_storage)
                {
                    v._Start = v._Last = v._End_storage = pointer();
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
                    _Start = v._Start;
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
                    : alloc_type(std::move(alloc))
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

        protected:
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
                : Impl(std::move(alloc))
            {
                _Create_storage(n);
            }

            Vector_base(Vector_base &&v, allocator_type const &alloc)
                : Impl(std::move(alloc))
            {
                if (alloc == v.get_allocator())
                    this->Impl._swap(v);
                else
                {
                    size_type n = v.Impl._End_storage - v.Impl._Start;
                    _Create_storage(n);
                }
            }

            ~Vector_base()
            {
                _Deallocate(Impl._Start, Impl._End_storage - Impl._Start);
            }

        protected:
            Vector_impl Impl;

            [[nodiscard]] pointer _Allocate(size_type n) noexcept
            {
                return n ? std::allocator_traits<alloc_type>::allocate(Impl, n) : pointer();
            }

            void _Deallocate(pointer ptr, size_type n) noexcept
            {
                if (ptr)
                    std::allocator_traits<alloc_type>::deallocate(Impl, ptr, n);
            }

            size_type n;

        protected:
            void _Create_storage(size_type n) noexcept
            {
                Impl._Start = _Allocate(n);
                Impl._Last = Impl._Start;
                Impl._End_storage = Impl._Start + n;
            }
        };

        template <typename _Ty>
        class Vector_const_iterator
        {
        public:
            using value_type = _Ty;
            using reference = value_type&;
            using pointer = value_type*;
            using diference_type = ptrdiff_t;
            using iterator_category = std::bidirectional_iterator_tag;

            using Self = Vector_const_iterator<_Ty>;
            
            Vector_const_iterator(pointer ptr)
                : _Current(ptr)
            {
            }

            reference operator*() const noexcept
            {
                return *operator->();
            }

            pointer operator->() const noexcept
            {
                return _Current;
            }

            Self operator++() noexcept
            {
                return Self(_Current++);
            }

            Self operator++(int) noexcept
            {
                _Current++;
                return *this;
            }

            reference operator[](const diference_type& offset) noexcept
            {
                return _Current[offset];
            }

            Self operator+(const diference_type& offset) const noexcept
            {
                return Self(_Current + offset);
            }

            Self operator+=(const diference_type& offset) noexcept
            {
                _Current += offset;
                return *this;
            }

            Self operator-(const diference_type& offset) const noexcept
            {
                return this + -offset;
            }

            Self operator-=(const diference_type& offset) noexcept
            {
                return operator+=(-offset);
            }

        private:
            pointer _Current;
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
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using const_iterator = __base::Vector_const_iterator<_Ty>;
    protected:
        using _Base::_Allocate;
        using _Base::_Deallocate;
        using _Base::_Get_allocator;
        using _Base::Impl;

        static size_type _S_check_size_init(size_type n, allocator_type const &alloc)
        {
            const size_type diffmax = std::numeric_limits<ptrdiff_t>::max() / sizeof(_Ty);
            const size_type allocmax = alloc_traits::max_size(alloc);
            const size_type small = std::min(diffmax, allocmax);

            if (n > small)
            std:
                __throw_length_error("cannot create collections::vector larger than max_size()");

            return n;
        }

    public:
        vector() = default;
        
        ~vector() noexcept
        {
            std::_Destroy(this->Impl._Start, this->Impl._Last, _Get_allocator());
        }
        /**
         * @brief Construct a new vector no elements
         * 
         * @param alloc An allocator to vector
         */
        explicit vector(allocator_type const &alloc) noexcept
            : _Base(alloc)
        {
        }

        /**
         * @brief Construct a new vector of @a n size with the value of @a value 
         * 
         * @param n The Number of elementy to initially create.
         * @param value An element to copy
         * @param alloc An allocator
         */
        explicit vector(size_type n, value_type const &value = value_type(), allocator_type const &alloc = allocator_type()) noexcept
            : _Base(_S_check_size_init(n, alloc), alloc)
        {
            _Fill_initialize(n, value);
        }

        vector(vector const &vec) noexcept
        {
            this->Impl._Last = std::__uninitialized_fill_n_a(
                vec.begin(), vec.end(), this->Impl._Start, _Get_allocator());
        }

        const_iterator cbegin() noexcept
        {
            return const_iterator(this->Impl._Start);
        }

    private:
        void _Fill_initialize(size_type n, value_type const &value) noexcept
        {
            this->Impl._Last = std::__uninitialized_fill_n_a(this->Impl._Start,
                                                             n, value, _Get_allocator());
        }
    };
}