#include <memory>
#include <initializer_list>

namespace collections
{
    namespace __base
    {
        class List_node_base
        {
        public:
            List_node_base *_Prev;
            List_node_base *_Next;

            static void swap(List_node_base &x, List_node_base &y) noexcept
            {
                if (x._Next != &x)
                {
                    if (y._Next != &y)
                    {
                        std::swap(x._Next, y._Next);
                        std::swap(x._Prev, y._Prev);
                        x._Next->_Prev = x._Prev->_Next = &x;
                        y._Next->_Prev = y._Prev->_Next = &y;
                    }
                    else
                    {
                        y._Next = x._Next;
                        y._Prev = x._Prev;
                        y._Next->_Prev = y._Prev->_Next = &y;
                        x._Next = x._Prev = &x;
                    }
                }
                else if (y._Next != &y)
                {
                    x._Next = y._Next;
                    x._Prev = y._Prev;
                    x._Next->_Prev = x._Prev->_Next = &x;
                    y._Next = y._Prev = &y;
                }
            }

            void _Transfer(List_node_base *const first, List_node_base *const last) noexcept
            {
                if (this != last)
                {
                    last->_Prev->_Next = this;
                    first->_Prev->_Next = last;
                    this->_Prev->_Next = first;

                    // Splice [first, last] into its new position.
                    List_node_base *const temp = this->_Prev;
                    this->_Prev = last->_Prev;
                    last->_Prev = first->_Prev;
                    first->_Prev = temp;
                }
            }

            void _Reverse() noexcept
            {
                List_node_base *temp = this;
                do
                {
                    std::swap(temp->_Next, temp->_Prev);

                    // Old next node is now prev.
                    temp = temp->_Prev;
                } while (temp != this);
            }

            void _Hook(List_node_base *const position) noexcept
            {
                this->_Next = position;
                this->_Prev = position->_Prev;
                position->_Prev->_Next = this;
                position->_Prev = this;
            }

            void _Unhook() noexcept
            {
                List_node_base *const next = this->_Next;
                List_node_base *const prev = this->_Prev;
                prev->_Next = next;
                next->_Prev = prev;
            }
        };

        class List_node_header
            : public List_node_base
        {
        public:
            size_t _Size;

            List_node_header() noexcept
            {
                _Init();
            }

            List_node_header(List_node_header &&_x) noexcept
                : List_node_base{_x._Prev, _x._Next}, _Size(_x._Size)
            {
                if (_x._Base()->_Next == _x._Base())
                {
                    this->_Next = this->_Prev = this;
                }
                else
                {
                    this->_Next->_Prev = this->_Prev->_Next = this->_Base();
                    _x._Init();
                }
            }

            void _Move_nodes(List_node_header &&_x)
            {
                List_node_base *const _xnode{_x._Base()};

                if (_xnode->_Next == _xnode)
                {
                    _Init();
                }
                else
                {
                    List_node_base *_node{this->_Base()};

                    _node->_Next = _xnode->_Next;
                    _node->_Prev = _xnode->_Prev;
                    _node->_Next->_Prev = _node->_Prev->_Next = _node;
                    this->_Size = _x._Size;

                    _x._Init();
                }
            }

            void _Init() noexcept
            {
                this->_Next = this->_Prev = this;
                this->_Size = 0;
            }

        private:
            List_node_base *_Base()
            {
                return this;
            }
        };

        template <class _Ty>
        class List_node
            : public List_node_base
        {
        public:
            _Ty *_Valptr()
            {
                return _Data._M_ptr();
            }

            _Ty *const _Valptr() const
            {
                return _Data._M_ptr();
            }

        private:
            __gnu_cxx::__aligned_membuf<_Ty> _Data;
        };

        template <class _Ty>
        class List_iterator;

        template <class _Ty>
        class List_const_iterator
        {
        public:
            using Self = __base::List_const_iterator<_Ty>;
            using _Node = List_node<_Ty>;

            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type = ptrdiff_t;
            using value_type = _Ty;
            using pointer = _Ty *;
            using reference = _Ty &;

            List_iterator<_Ty> _Const_cast() const noexcept
            {
                return List_iterator<_Ty>(const_cast<List_node_base *>(_M_node));
            }

            List_const_iterator()
                : _M_node()
            {
            }

            List_const_iterator(const __base::List_node_base* x)
                : List_const_iterator(const_cast<List_node_base*>(x))
            {

            }

            List_const_iterator(__base::List_node_base *x)
                : _M_node(x)
            {
            }

            List_const_iterator(__base::List_iterator<_Ty>& x)
                : _M_node(x._M_node)
            {
            }

            reference operator*() const noexcept
            {
                return *static_cast<_Node *>(_M_node)->_Valptr();
            }

            pointer operator->() const noexcept
            {
                return static_cast<_Node *>(_M_node)->_Valptr();
            }

            Self &operator++() noexcept
            {
                _M_node = _M_node->_Next;
                return *this;
            }

            Self &operator++(int) noexcept
            {
                Self temp{*this};
                _M_node = _M_node->_Next;
                return temp;
            }

            Self &operator--() noexcept
            {
                _M_node = _M_node->_Prev;
                return *this;
            }

            Self &operator--(int) noexcept
            {
                Self temp{*this};
                _M_node = _M_node->_Prev;
                return temp;
            }

            friend bool operator==(const Self &_x, const Self &_y) noexcept
            {
                return _x._M_node == _y._M_node;
            }

            friend bool operator!=(const Self &_x, const Self &_y) noexcept
            {
                return _x._M_node != _x._M_node;
            }

            __base::List_node_base *_M_node;
        };

        template <class _Ty>
        class List_iterator
            : public List_const_iterator<_Ty>
        {
        public:
            using Self = List_iterator<_Ty>;
            using _Node = List_node<_Ty>;
            using _Base = List_const_iterator<_Ty>;

            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type = ptrdiff_t;
            using value_type = _Ty;
            using pointer = _Ty *;
            using reference = _Ty &;

            List_iterator() noexcept
                : _Base()
            {
            }

            List_iterator(__base::List_node_base *_x) noexcept
                : _Base(_x)
            {
            }

            Self _Cast() const noexcept
            {
                return const_cast<Self>(*this);
            }

            reference operator*() noexcept
            {
                return _Base::operator*();
            }

            pointer operator->() noexcept
            {
                return _Base::operator->();
            }

            Self operator++() noexcept
            {
                _Base::operator++();
                return *this;
            }

            Self operator++(int) noexcept
            {
                Self temp{*this};
                _Base::operator++();
                return temp;
            }

            Self operator--() noexcept
            {
                _Base::operator--();
                return *this;
            }

            Self operator--(int) noexcept
            {
                Self temp{*this};
                _Base::operator--();
                return temp;
            }

            friend bool operator==(const Self &_x, const Self &_y) noexcept
            {
                return _x._M_node == _y._M_node;
            }

            friend bool operator!=(const Self &_x, const Self &_y) noexcept
            {
                return _x._M_node != _y._M_node;
            }
        };

        template <class _Ty, class _Alloc>
        class List_base
        {
        protected:
            using alloc_t = typename std::allocator_traits<_Alloc>::template rebind_alloc<_Ty>;
            using allocator_traits = std::allocator_traits<alloc_t>;
            using node_alloc_t = typename allocator_traits::template rebind_alloc<List_node<_Ty>>;
            using node_alloc_traits = std::allocator_traits<node_alloc_t>;

            static size_t _Distance(__base::List_node_base *first, __base::List_node_base *second)
            {
                size_t i = 0;
                while (first != second)
                {
                    first = first->_Next;
                    i++;
                }
                return i;
            }

            struct List_impl
                : public node_alloc_t
            {
            public:
                __base::List_node_header _M_node;

                List_impl() noexcept(std::is_nothrow_default_constructible<node_alloc_t>::value)
                    : node_alloc_t()
                {
                }

                List_impl(node_alloc_t &alloc) noexcept
                    : node_alloc_t(std::move(alloc))
                {
                }

                List_impl(const node_alloc_t &alloc) noexcept
                    : List_impl(alloc)
                {
                }
            };

            List_impl Impl;

            size_t _Get_size() const
            {
                return Impl._M_node._Size;
            }

            size_t _Node_count()
            {
                return _Distance(Impl._M_node._Next, std::addressof(Impl._M_node));
            }

            typename node_alloc_traits::pointer _Get_node()
            {
                return node_alloc_traits::allocate(Impl, 1);
            }

            void _Put_node(typename node_alloc_traits::pointer ptr) noexcept
            {
                node_alloc_traits::deallocate(Impl, ptr, 1);
            }

            void _Clear() noexcept
            {
                List_node_base *cur = Impl._M_node._Next;
                while (cur != Impl._M_node)
                {
                    List_node<_Ty> *temp = static_cast<List_node<_Ty>>(cur);
                    cur = temp->_Next;
                    _Ty *val = temp->_Valptr();
                    node_alloc_traits::destroy(_Get_node_allocator(), val);
                    _Put_node(temp);
                }
            }

            void _Set_size(size_t n)
            {
                Impl._M_node._Size = n;
            }

            void _Inc_size(size_t n)
            {
                Impl._M_node._Size += n;
            }

            void _Dec_size(size_t n)
            {
                Impl._M_node._Size -= n;
            }

        public:
            using allocator_type = _Alloc;

            node_alloc_t &_Get_node_allocator() noexcept
            {
                return Impl;
            }

            const node_alloc_t &_Get_node_allocator() const noexcept
            {
                return Impl;
            }

            List_base(const node_alloc_t &alloc) noexcept
                : Impl(alloc)
            {
            }

            List_base(node_alloc_t&& alloc) noexcept
                :Impl(alloc)
            {

            }

            List_base()
            {
            }

            void _Init()
            {
                this->Impl._M_node._Init();
            }
        };
    }

    template <class _Ty, class _Alloc = std::allocator<_Ty>>
    class list : protected __base::List_base<_Ty, _Alloc>
    {
        using _Base = __base::List_base<_Ty, _Alloc>;
        using alloc_t = typename _Base::alloc_t;
        using allocator_traits = typename _Base::allocator_traits;
        using node_alloc_t = typename _Base::node_alloc_t;
        using node_alloc_traits = typename _Base::node_alloc_traits;

    public:
        using allocator_type = _Alloc;
        using difference_type = ptrdiff_t;
        using size_type = size_t;
        using value_type = _Ty;
        using pointer = typename allocator_traits::pointer;
        using reference = _Ty &;
        using const_pointer = typename allocator_traits::const_pointer;
        using const_reference = const reference;

        using iterator = __base::List_iterator<_Ty>;
        using const_iterator = __base::List_const_iterator<_Ty>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        using reverse_iterator = std::reverse_iterator<iterator>;

    protected:
        using _Node = __base::List_node<_Ty>;

        using _Base::_Get_node;
        using _Base::_Get_node_allocator;
        using _Base::_Put_node;
        using _Base::Impl;

        template <typename... _Args>
        _Node *
        _Create_node(_Args &&...__args)
        {
            auto __p = this->_Get_node();
            auto &__alloc = _Get_node_allocator();
            std::__allocated_ptr<node_alloc_t> __guard{__alloc, __p};
            node_alloc_traits::construct(__alloc, __p->_Valptr(),
                                         std::forward<_Args>(__args)...);
            __guard = nullptr;
            return __p;
        }

        static size_t _Distance(const_iterator first, const_iterator last)
        {
            return std::distance(first, last);
        }

        size_t _Node_count() const
        {
            return this->_Get_size();
        }

    public:
        /**
         * @brief Construct a new list object with no elements
         * 
         */
        list()
        {
        }

        /**
         * @brief Construct a new list object no elements
         * 
         * @param alloc 
         */
        explicit list(const allocator_type *alloc) noexcept
            : _Base(node_alloc_t(alloc))
        {
        }

        /**
         * @brief Construct a new list object no elements
         * 
         * @param alloc 
         */
        explicit list(const allocator_type &alloc) noexcept
            : _Base(node_alloc_t(alloc))
        {
        }

        /**
         * @brief Construct a new list object with default constructed elements.
         * 
         * @param n The number of elements to initially create.
         * @param alloc An allocator object.
         */
        explicit list(size_t n, const allocator_type &alloc = allocator_type())
            : _Base(_node_alloc_t(alloc))
        {
            _Default_append(n);
        }

        /**
       *  @brief  Crostruct a new list with copies of an exemplar element.
       *  @param  n  The number of elements to initially create.
       *  @param  value  An element to copy.
       *  @param  a  An allocator object.
       *
       *  This constructor fills the list with @a n copies of @a value.
       */
        explicit list(size_type n, value_type &&value = value_type(),
                      const allocator_type &alloc = allocator_type())
            : _Base(node_alloc_t(alloc))
        {
            _Fill_initialize(n, value);
        }

        /**
         * @brief Construct a new list with copies of range [first, last)
         * 
         * @param first 
         * @param last 
         */
        explicit list(const_iterator &first, const_iterator &last,
                      const allocator_type &alloc = allocator_type())
            : _Base(node_alloc_t(alloc))
        {
            _Fill_initialize(first, last, std::false_type());
        }

        /**
         * @brief Construct a new list object
         * 
         * @tparam Input 
         * @tparam typename 
         * @param first 
         * @param last 
         * @param alloc 
         */
        template <typename Input, typename = std::_RequireInputIter<Input>>
        list(Input first, Input last, const allocator_type &alloc = allocator_type())
            : _Base(node_alloc_t(alloc))
        {
            _Fill_initialize(first, last);
        }

        list(std::initializer_list<value_type> l, const allocator_type &alloc = allocator_type())
            : _Base(node_alloc_t(alloc))
        {
            _Fill_initialize(l.begin(), l.end());
        }

        /**
         * @brief 
         * 
         * @param n 
         * @param value 
         */
        void assign(size_t n, const value_type &value)
        {
            iterator it = begin();
            while (--n && it != end())
            {
                *it = value;
            }
            if (n > 0)
                insert(end(), n, value);
        }

        /**
         * @brief Constructs the new elements from each of the elements in the range between first and last, in the same order.
         * 
         * @tparam Input 
         * @param first 
         * @param last 
         */
        template <typename Input, typename = std::_RequireInputIter<Input>>
        void assign(Input first, Input last)
        {
            iterator mfirst = begin();
            iterator mlast = end();

            while (first != last && mfirst != mlast)
                *mfirst++ = *first++;
            if (first == last)
                this->erase(mfirst, mlast);
            else
                this->insert<Input, std::_RequireInputIter<Input>>(mlast, first, last);
        }

        /**
         * @brief The new contents are copies of the values passed as initializer list, in the same order
         * 
         * @param l 
         */
        void assign(std::initializer_list<value_type> l)
        {
            this->assign(l.begin(), l.end());
        }

        /**
         * @brief Get an reference to end of the list 
         * 
         * @return reference 
         */
        reference back() noexcept
        {
            iterator temp = end();
            --temp;
            return *temp;
        }

        /**
         * @brief Get an constante reference to end of the list 
         * 
         * @return reference 
         */
        const_reference back() const noexcept
        {
            iterator temp = end();
            --temp;
            return *temp;
        }

        /**
         * @brief Get an iterator to begin of the list
         * 
         * @return iterator 
         */
        iterator begin() noexcept
        {
            return iterator(this->Impl._M_node._Next);
        }

        /**
         * @brief Get an constant iterator to begin of the list
         * 
         * @return const_iterator 
         */
        const_iterator begin() const noexcept
        {
            return const_iterator(this->Impl._M_node._Next);
        }

        /**
         * @brief Get an constant iterator to begin of the list
         * 
         * @return const_iterator 
         */
        const_iterator cbegin() const noexcept
        {
            return const_iterator(this->Impl._M_node._Next);
        }

        /**
         * @brief Get an constant iterator to end of the list
         * 
         * @return const_iterator 
         */
        const_iterator cend() const noexcept
        {
            return const_iterator(this->Impl._M_node);
        }

        /**
         * @brief Get an constant reverse iterator to begin of the list
         * 
         * @return const_iterator 
         */
        const_reverse_iterator crbegin() const noexcept
        {
            return const_reverse_iterator(begin());
        }

        /**
         * @brief Get an constant reverse iterator to end of the list
         * 
         * @return const_iterator 
         */
        const_reverse_iterator crend() const noexcept
        {
            return const_reverse_iterator(end());
        }

        void clear() const noexcept
        {
            _Base::_Clear();
            _Base::_Init();
        }

        /**
         * @brief Get an iterator to end of the list
         * 
         * @return iterator 
         */
        iterator end() noexcept
        {
            return iterator(&this->Impl._M_node);
        }

        /**
         * @brief Get an constant iterator to end of the list
         * 
         * @return const_iterator 
         */
        const_iterator end() const noexcept
        {
            return const_iterator(&this->Impl._M_node);
        }

        /**
         * @brief Inserts a new object in the end of list
         * 
         * @tparam Args 
         * @param args 
         * @return reference 
         */
        template <typename... Args>
        reference emplace_back(Args &&...args) noexcept
        {
            this->_Insert(end(), std::forward<Args>(args)...);
            return back();
        }

        /**
         * @brief Inserts a new object in the begin of list
         * 
         * @tparam Args 
         * @param args 
         * @return reference 
         */
        template <typename... Args>
        reference emplace_front(Args &&...args) noexcept
        {
            this->_Insert(begin(), std::forward<Args>(args)...);
            return front();
        }

        /**
         * @brief Check if the list is empty
         * 
         * @return true 
         * @return false 
         */
        bool empty() const noexcept
        {
            return this->Impl._M_node._Next == &this->Impl._M_node;
        }

        /**
         * @brief Remove element at given position.
         * 
         * @param position 
         * @return iterator 
         */
        iterator erase(const_iterator position) noexcept
        {
            iterator temp = iterator(position._M_node->_Next);
            this->_Erase(position._Const_cast());
            return temp;
        }

        /**
        *  @brief  Remove a range of elements.
        * 
        *  @param  first  Iterator pointing to the first element to be erased.
        *  @param  last  Iterator pointing to one past the last element to be
         */
        iterator erase(const_iterator first, const_iterator last) noexcept
        {
            while (first != last)
                first = erase(first);
            return last._Const_cast();
        }

        /**
         * @brief Gets a reference to begin
         * 
         * @return reference 
         */
        reference front()
        {
            iterator temp = begin();
            temp++;
            return *temp;
        }

        /**
         * @brief Get the allocator object
         * 
         * @return allocator_type 
         */
        allocator_type get_allocator() const noexcept
        {
            return allocator_type(_Base::_Get_node_allocator());
        }

        /**
         * @brief  Inserts given value into list before specified iterator
         * 
         * @param position Position to insert value
         * @param value Value to insert
         * @return iterator 
         */
        iterator insert(const_iterator position, value_type &&value)
        {
            return this->emplace(position, std::move(value));
        }

        /**
         * @brief Inserts a range into the list.
         * 
         * @tparam Input 
         * @tparam typename 
         * @param position Position to insert value
         * @param first Beggining range 
         * @param last Final range
         * @return iterator 
         */
        template <class Input, typename = std::_RequireInputIter<Input>>
        iterator insert(const_iterator position, Input first, Input last)
        {
            list temp(first, last, get_allocator());
            if (!temp.empty())
            {
                iterator it = temp.begin();
                this->splice(position, std::move(temp));
                return it;
            }
            return position._Const_cast();
        }

        /**
         * @brief Inserts the contents of an initializer_list into %list
         *        before specified const_iterator.
         * @param position An position to insert 
         * @param l An initializer_list of value_type
         * @return An iterator pointing to the first element inserted
         *         (or position).
         */
        iterator insert(const_iterator position, std::initializer_list<value_type> l)
        {
            return this->insert(position, l.begin(), l.end());
        }

        /**
         * @brief Inserts a number of copies of given data into the list.
         * 
         * @param position A const_iterator into the list.
         * @param n Number of elements to be inserted.
         * @param value Data to be inserted.
         * @return An iterator pointing to the first element inserted
         *         (or position).
         */
        iterator insert(const_iterator position, size_type n, const value_type &value) noexcept
        {
            if (n)
            {
                list temp(n, value, get_allocator());
                iterator it = temp.begin();
                splice(position, it);
                return it;
            }
            return position._Const_cast();
        }

        /**
         * @brief Returns the size() of the largest possible %list.
         * 
         * @return size_type 
         */
        size_type max_size() noexcept
        {
            return node_alloc_traits::max_size(_Get_node_allocator());
        }

        /**
         * @brief Merge sorted lists
         * 
         * @param x 
         */
        void merge(list &&x)
        {
            if (this != std::__addressof(x))
            {
                iterator first1 = begin();
                iterator last1 = end();
                iterator first2 = x.begin();
                iterator last2 = x.end();
                const size_t orig_size = x.size();
                try
                {
                    while (first1 != last1 && first2 != last2)
                        if (*first2 < *first1)
                        {
                            iterator next = first2;
                            _Transfer(first1, first2, ++next);
                            first2 = next;
                        }
                        else
                            ++first1;
                    if (first2 != last2)
                        _Transfer(last1, first2, last2);

                    this->_Inc_size(x._Get_size());
                    x._Set_size(0);
                }
                catch (...)
                {
                    const size_t dist = std::distance(first2, first1);
                    this->_Inc_size(orig_size - dist);
                    x._Set_size(dist);
                    throw;
                }
            }
        }

        /**
         * @brief Merge sorted lists
         * 
         * @param x 
         */
        void merge(list &x) noexcept
        {
            merge(std::move(x));
        }

        /**
         * @brief  Merge sorted lists according to comparison function.
         * 
         * @tparam Input 
         * @param x 
         * @param comp 
         */
        template <class Input>
        void merge(list &&x, Input comp)
        {
            if (this != std::__addressof(x))
            {
                iterator first1 = begin();
                iterator last1 = end();
                iterator first2 = x.begin();
                iterator last2 = x.end();
                const size_t orig_size = x.size();
                try
                {
                    while (first1 != last1 && first2 != last2)
                        if (comp(*first2, *first1))
                        {
                            iterator next = first2;
                            _Transfer(first1, first2, ++next);
                            first2 = next;
                        }
                        else
                            ++first1;
                    if (first2 != last2)
                        _Transfer(last1, first2, last2);

                    this->_Inc_size(x._Get_size());
                    x._Set_size(0);
                }
                catch (...)
                {
                    const size_t dist = std::distance(first2, first1);
                    this->_Inc_size(orig_size - dist);
                    x._Set_size(dist);
                    throw;
                }
            }
        }

        /**
         * @brief  Merge sorted lists according to comparison function.
         * 
         * @tparam Input 
         * @param x 
         * @param comp 
         */
        template <class Input>
        void merge(list &x, Input comp)
        {
            merge(std::move(x), comp);
        }

        /**
         * @brief Removes first element.
         * 
         */
        void pop_front() noexcept
        {
            this->_Erase(begin());
        }

        /**
         * @brief Removes last element.
         * 
         */
        void pop_back() noexcept
        {
            this->_Erase(Impl._M_node._Prev);
        }

        /**
         * @brief Inserts a new value in the end of list
         * 
         * @param value 
         */
        void push_back(value_type &&value) noexcept
        {
            this->_Insert(end(), std::move(value));
        }

        /**
         * @brief Gets a reverse iterator to list
         * 
         * @return reverse_iterator 
         */
        reverse_iterator rbegin() const noexcept
        {
            return reverse_iterator(begin());
        }

        /**
         * @brief Gets a reverse iterator to list
         * 
         * @return reverse_iterator 
         */
        reverse_iterator rend()
        {
            return reverse_iterator(end());
        }

        /**
         * @brief Removes every element in the list equal to @a value.
         *        Remaining elements stay in list order.
         * 
         * @param value 
         * @return size_type 
         */
        size_type remove(const value_type &value) noexcept
        {
            size_type removed = 0;
            iterator first = begin();
            iterator last = end();
            iterator extra = last;

            while (first != last)
            {
                iterator next = first;
                ++next;
                if (*next = value)
                {
                    if (std::addressof(first) != std::addressof(value))
                    // check if the value found is the base value
                    {
                        _Erase(first);
                        removed++;
                    }
                    else
                        extra = first;
                }
                first = next;
            }
            if (extra != last)
            {
                _Erase(extra);
                removed++;
            }
            return removed;
        }

        /**
         * @brief Removes every element in the list for which the predicate
         *        returns true
         * 
         * @tparam Predicate 
         * @param pred 
         * @return size_type 
         */
        template <class Predicate>
        size_type remove_if(Predicate pred) noexcept
        {
            size_type removed = 0;
            iterator first = begin();
            iterator last = end();
            while (first != last)
            {
                iterator next = first;
                ++next;
                if (pred(*first))
                {
                    _Erase(first);
                    removed++;
                }
                first = next;
            }
            return removed;
        }

        /**
         * @brief  Resizes the list to the specified number of elements.
         * 
         * @param new_size 
         */
        void resize(size_type new_size) noexcept
        {
            const_iterator it = this->_Resine_pos(new_size);

            if (new_size)
                _Default_append(new_size);
            else
                erase(it, end());
        }

        /**
         * @brief Resizes the list to the specified number of elements.
         * 
         * @param new_size 
         * @param value 
         */
        void resize(size_type new_size, const value_type &value) noexcept
        {
            const_iterator it = this->_Resize_pos(new_size);
            if (new_size)
                insert(end(), new_size, value);
            else
                erase(it, end());
        }

        /** 
         *  @brief  Reverse the elements in list.
         *
         *  Reverse the order of elements in the list in linear time.
         */
        void reverse() const noexcept
        {
            this->Impl._M_node._Reverse();
        }

        /**
         * @brief Gets the size of the list 
         * 
         * @return size_type 
         */
        size_type size()
        {
            return this->_Get_size();
        }

        /**
         * @brief Transfers elements from x into the container, inserting them at position.
         * 
         * @param position An position to insert 
         * @param l An initializer_list of value_type
         */
        void splice(const_iterator position, list &&l) noexcept
        {
            if (!l.empty())
            {
                _Compare_allocators(l);

                _Transfer(position._Const_cast(), l.begin(), l.end());
                this->_Inc_size(l._Get_size());
                l._Set_size(0);
            }
        }

        /**
         * @brief Transfers elements from x into the container, inserting them at position.
         * 
         * @param position An position to insert 
         * @param l An initializer_list of value_type
         */
        void splice(const_iterator position, list &l) noexcept
        {
            splice(position, std::move(l));
        }

        /**
         * @brief Transfers only the element pointed by i from l into the position.
         * 
         * @param position An position to insert 
         * @param l An initializer_list of value_type
         * @param i The position to be inserted
         */
        void splice(const_iterator position, list &&l, const_iterator i) noexcept
        {
            iterator j = i._Const_cast();
            ++j;
            if (position == j || position == i)
                return;
            if (this != std::addressof(l))
                _Compare_allocators(l);
            _Transfer(position._Const_cast(), i._Const_cast(), j);

            this->_Inc_size(1);
            l._Dec_size(1);
        }

        /**
         * @brief Transfers only the element pointed by i from l into the position.
         * 
         * @param position An position to insert 
         * @param l An initializer_list of value_type
         * @param i The position to be inserted
         */
        void splice(const_iterator position, list &l, const_iterator i) noexcept
        {
            splice(position, std::move(l), i);
        }

        /**
         * @brief Transfers the range [first,last) from l into the container.
         * 
         * @param position An position to insert 
         * @param l An initializer_list of value_type
         * @param first Beggining range 
         * @param last Final range
         */
        void splice(const_iterator position, list &&l, const_iterator first, const_iterator last) noexcept
        {
            if (first != last)
            {
                if (this != std::addressof(l))
                    _Compare_allocators(l);

                size_t n = _Distance(first, last);
                this->_Inc_size(n);
                l._Dec_size(n);

                this->_Transfer(position._Const_cast(), first._Const_cast(), last._Const_cast());
            }
        }

        /**
         * @brief Transfers the range [first,last) from l into the container.
         * 
         * @param position An position to insert 
         * @param l An initializer_list of value_type
         * @param first Beggining range 
         * @param last Final range
         */
        void splice(const_iterator position, list &l, const_iterator first, const_iterator last) noexcept
        {
            splice(position, std::move(l), first, last);
        }

        /**
         * @brief Sorts the elements of this list. Equivalent elements remain in list order.
         * 
         */
        void sort() noexcept
        {
            if (this->Impl._M_node._Next != &this->Impl._M_node && this->Impl._M_node._Next->_Next != &this->Impl._M_node)
            {
                list carry;
                list temp[64];
                list *fill = temp;
                list *counter;

                try
                {
                    do
                    {
                        carry.splice(carry.begin(), *this, begin());

                        for (counter = temp; counter != fill && !counter->empty(); ++counter)
                        {
                            counter->merge(carry);
                            carry.swap((*counter));
                        }
                        carry.swap((*counter));
                        if (counter == fill)
                            ++fill;
                    } while (!empty());

                    for (counter = temp + 1; counter != fill; ++counter)
                        counter->merge(*(counter - 1));
                    swap(*(fill - 1));
                }
                catch (...)
                {
                    this->splice(this->end(), carry);
                    for (int i = 0; i < sizeof(temp) / sizeof(temp[0]); ++i)
                        this->splice(this->end(), temp[i]);
                    __throw_exception_again;
                }
            }
        }

        /**
         * @brief Sort the elements according to comparison function.t. Equivalent elements remain in list order.
         * 
         */
        template <class Input>
        void sort(Input comp) noexcept
        {
            if (this->Impl._M_node._Next != &this->Impl._M_node && this->Impl._M_node._Next->_Next != &this->Impl._M_node)
            {
                list carry;
                list temp[64];
                list *fill = temp;
                list *counter;

                try
                {
                    do
                    {
                        carry.splice(carry.begin(), *this, begin());

                        for (counter = temp; counter != fill && !counter->empty(); ++counter)
                        {
                            counter->merge(carry, comp);
                            carry.swap((*counter));
                        }
                        carry.swap((*counter));
                        if (counter == fill)
                            ++fill;
                    } while (!empty());

                    for (counter = temp + 1; counter != fill; ++counter)
                        counter->merge(*(counter - 1), comp);
                    swap(*(fill - 1));
                }
                catch (...)
                {
                    this->splice(this->end(), carry);
                    for (int i = 0; i < sizeof(temp) / sizeof(temp[0]); ++i)
                        this->splice(this->end(), temp[i]);
                    __throw_exception_again;
                }
            }
        }

        /**
         * @brief Swaps data with another list.
         * 
         * @param x 
         */
        void swap(list &x) noexcept
        {
            __base::List_node_base::swap(this->Impl._M_node, x.Impl._M_node);

            size_t xsize = x._Get_size();
            x._Set_size(this->_Get_size());
            this->_Set_size(xsize);

            std::swap(this->_Get_node_allocator(), x._Get_node_allocator());
        }

        /**
         *  @brief  Remove consecutive duplicate elements.
         * 
         * @return size_type 
         */
        size_type unique() noexcept
        {
            iterator first = begin();
            iterator last = end();

            if (first == last)
                return 0;

            size_type removed = 0;
            iterator next = first;
            while (++next != last)
            {
                if (*first == *next)
                {
                    _Erase(next);
                    removed++;
                }
                else
                    first = next;
                next = first;
            }
            return removed;
        }

        /**
         * @brief  Remove consecutive elements satisfying a predicate.
         * 
         * @tparam Input 
         * @param bin 
         * @return size_type 
         */
        template <class Input>
        size_type unique(Input bin) noexcept
        {
            iterator first = begin();
            iterator last = end();

            if (first != last)
                return 0;

            size_type removed = 0;
            iterator next = first;
            while (next != last)
            {
                if (bin(*first, *next))
                {
                    _Erase(next);
                    removed++;
                }
                else
                    first = next;
                next = first;
            }
            return removed;
        }

    private:
        void _Compare_allocators(list &li)
        {
            if (std::__alloc_neq<typename _Base::node_alloc_t>::_S_do_it( // read more at libstdc++-v3/include/bits/allocator.h
                    _Get_node_allocator(), li._Get_node_allocator()))
                __builtin_abort();
        }

        void _Default_append(size_type size)
        {
            while (size--)
            {
                emplace_back();
            }
        }

        void _Erase(const_iterator position)
        {
            this->_Dec_size(1);
            position._M_node->_Unhook();
            _Node *node = static_cast<_Node *>(position._M_node);
            node_alloc_traits::destroy(_Get_node_allocator(), node->_Valptr());
            this->_Put_node(node);
        }

        template <typename Input>
        void _Fill_initialize(Input first, Input last)
        {
            for (; first != last; first++)
                emplace_back(*first);
        }

        template <typename... Args>
        void _Insert(iterator position, Args &&...value)
        {
            _Node *temp = _Create_node(std::forward<Args>(value)...);
            temp->_Hook(position._M_node);
            this->_Inc_size(1);
        }

        void _Transfer(iterator position, iterator first, iterator end)
        {
            position._M_node->_Transfer(first._M_node, end._M_node);
        }

        iterator _Resine_pos(size_type &new_size) const
        {
            const_iterator ret;
            const size_type len = this->_Get_size();
            if (new_size < len)
            {
                if (new_size <= len / 2)
                {
                    ret = begin();
                    std::advance(ret, new_size);
                }
                else
                {
                    ret = end();
                    difference_type num_erase = len - new_size;
                    std::advance(ret, -num_erase);
                }
                new_size = 0;
                return ret;
            }
            else
                ret = end();

            new_size -= len;
            return ret;
        }
    };

    template <class _Ty, class _Alloc>
    inline bool operator==(list<_Ty, _Alloc> &l1, list<_Ty, _Alloc> &l2)
    {
        using list = list<_Ty, _Alloc>;
        using const_iterator =  typename list::const_iterator;
        // check size
        if (l1.size() != l2.size())
            return false;

        // check content
        const_iterator it1 = l1.begin();
        const_iterator it2 = l2.begin();
        const_iterator last1 = l1.end();
        const_iterator last2 = l2.end();

        for (; it1 != last1 && it2 != last2; ++it1, ++it2)
            ;

        return it1 == last1 && it2 == last2;
    }

    template <typename _Ty, typename _Alloc>
    inline bool operator<(const list<_Ty, _Alloc> &x, const list<_Ty, _Alloc> &y)
    {
        return std::lexicographical_compare(x.begin(), x.end(),
                                            y.begin(), y.end());
    }

    template <typename _Ty, typename _Alloc>
    inline bool operator>(const list<_Ty, _Alloc> &x, const list<_Ty, _Alloc> &y)
    {
        return y < x;
    }

    template <typename _Ty, typename _Alloc>
    inline bool operator!=(const list<_Ty, _Alloc> &x, const list<_Ty, _Alloc> &y)
    {
        return !(x == y);
    }

    template <typename _Ty, typename _Alloc>
    inline bool operator<=(const list<_Ty, _Alloc> &x, const list<_Ty, _Alloc> &y)
    {
        return !(y < x);
    }

    template <typename _Ty, typename _Alloc>
    inline bool operator>=(const list<_Ty, _Alloc> &x, const list<_Ty, _Alloc> &y)
    {
        return !(x > y);
    }
}