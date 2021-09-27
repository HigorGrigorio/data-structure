#include "../../include/pch.h"

namespace collections
{
    namespace iterators
    {
        /**
         * @brief Crray constant iterator class
         * 
         * @tparam T - type 
         * @tparam _size - predefined size
         */
        template <class T, size_t _size>
        class array_const_iterator
        {
        public:
            // useds by std::reverse iterator
            using iterator_category = std::random_access_iterator_tag; // definid in xutility
            using value_type = T;
            using difference_type = ptrdiff_t;
            using pointer = T *;
            using reference = T &;

            /**
             * @brief Construct a new array const iterator object
             * 
             */
            array_const_iterator()
                : array_const_iterator(nullptr, 0)
            {
            }

            /**
             * @brief Construct a new array const iterator object
             * 
             * @param data 
             * @param index 
             */
            constexpr explicit array_const_iterator(pointer data, size_t index) noexcept
                : _data(data), _index(index)
            {
            }

            /**
             * @brief Gives acess to element data for a pointer
             * 
             * 
             * @return constexpr reference 
             */
            constexpr reference operator*() const noexcept
            {
                return *operator->();
            }

            /**
             * @brief Gives acess to element methods for a pointer
             * 
             * @return constexpr pointer 
             */
            constexpr pointer operator->() const noexcept
            {
                _ASSERT_EXPR(0 < _index, "cannot seek array iterator before begin");
                _ASSERT_EXPR(_data, "cannot seek value-initialized array iterator");
                return _data + _index;
            }

            /**
             * @brief Add to index
             * 
             * @return constexpr array_const_iterator 
             */
            constexpr array_const_iterator operator++() noexcept
            {
                _ASSERT_EXPR(_data, "cannot seek value-initialized array iterator");
                _ASSERT_EXPR(_index >= _size, "cannot seek array iterator before begin");

                ++_index;
                return *this;
            }

            /**
             * @brief Add to index
             * 
             * @return constexpr array_const_iterator 
             */
            constexpr array_const_iterator operator++(int) noexcept
            {
                array_const_iterator temp = *this;

                ++(*this);
                return temp;
            }

            /**
             * @brief Decrements to index
             * 
             * @return constexpr array_const_iterator 
             */
            constexpr array_const_iterator operator--() noexcept
            {
                _ASSERT_EXPR(_data, "cannot seek value-initialized array iterator");
                _ASSERT_EXPR(0 < _index, "cannot seek array iterator after end");

                --_index;
                return *this;
            }

            /**
             * @brief Decrement to index
             * 
             * @return constexpr array_const_iterator 
             */
            constexpr array_const_iterator operator--(int) noexcept
            {
                array_const_iterator temp = *this;

                --*this;
                return temp;
            }

            /**
             * @brief Add to index a especfic off set
             * 
             * @param offset - diference pointer type
             * @return constexpr array_const_iterator 
             */
            [[nodiscard]] constexpr array_const_iterator &operator+=(const ptrdiff_t offset) noexcept
            {
                check_offset(offset);

                _index += static_cast<size_t>(offset);
                return *this;
            }

            /**
             * @brief Add to index a especfic off set
             * 
             * @param offset 
             * @return constexpr array_const_iterator 
             */
            constexpr array_const_iterator operator+(const ptrdiff_t offset) noexcept
            {
                array_const_iterator temp = *this;

                return temp += offset;
            }

            /**
             * @brief Decrease to index a especfic off set
             * 
             * @param offset 
             * @return constexpr array_const_iterator 
             */
            constexpr array_const_iterator operator-=(const ptrdiff_t offset) noexcept
            {
                return *this += -offset;
            }

            /**
             * @brief Decrease to index a especfic off set
             * 
             * @param offset 
             * @return constexpr array_const_iterator 
             */
            [[nodiscard]] constexpr array_const_iterator operator-(const ptrdiff_t offset) noexcept
            {
                array_const_iterator temp = *this;

                return temp -= offset;
            }

            /**
             * @brief Return a diference between arrays
             * 
             * @param right 
             * @return constexpr array_const_iterator 
             */
            [[nodiscard]] constexpr ptrdiff_t operator-(const array_const_iterator right) const noexcept
            {
                compatible(right);

                return static_cast<ptrdiff_t>(_index - right);
            }

            /**
             * @brief Give a reference data of the especifc off set
             * 
             * @param offset 
             * @return constexpr reference 
             */
            [[nodiscard]] constexpr reference operator[](const ptrdiff_t offset) const noexcept
            {
                return *(*this + offset);
            }

            /**
             * @brief Compare elements
             * 
             * @param rigth 
             * @return true 
             * @return false 
             */
            [[nodiscard]] constexpr bool operator!=(const array_const_iterator &right) const noexcept
            {
                return !(*this == right);
            }

            /**
             * @brief Compare arrays
             * 
             * @param rigth 
             * @return true 
             * @return false 
             */
            [[nodiscard]] constexpr bool operator==(const array_const_iterator &rigth) const noexcept
            {
                compatible(rigth);
                return _index == rigth._index;
            }

            /**
             * @brief Check if the content of the other array is compatible
             * 
             * @param other 
             */
            constexpr void compatible(const array_const_iterator &other) const noexcept
            {
                _ASSERT_EXPR(_data == other._data, "array iterators incompatible");
            }

            /**
             * @brief Check if the offset is valid
             * 
             * @param offset 
             */
            void check_offset(size_t offset) const noexcept
            {
                if (offset != 0)
                {
                    _ASSERT_EXPR(_data, "cannot seek value-initialized array iterator");
                }
                if (offset < 0)
                {
                    _ASSERT_EXPR(_index >= size_t{0} - static_cast<size_t>(offset), "cannot seek array iterator before begin");
                }
                if (offset > 0)
                {
                    _ASSERT_EXPR(_size - _index >= static_cast<size_t>(offset), "cannot seek array iterator afeter end");
                }
            }

        private:
            pointer _data;
            size_t _index;
        }; // const iterator

        /**
         * @brief Array_iterator Class
         * 
         * @tparam T 
         * @tparam _size 
         */
        template <class T, size_t _size>
        class array_iterator
            : public array_const_iterator<T, _size>
        {
        public:
            // useds by reverser iterator
            using _MyBase = array_const_iterator<T, _size>;

#ifdef __cpp_lib_concepts
            using iterator_concept = contiguous_iterator_tag;
#endif // __cpp_lib_concepts
            using iterator_category = std::random_access_iterator_tag;
            using value_type = T;
            using difference_type = ptrdiff_t;
            using pointer = T *;
            using reference = T &;
            /**
                 * @brief Construct a new array iterator object
                 * 
                 */
            array_iterator() noexcept
            {
            }

            /**
                 * @brief Construct a new array iterator object
                 * 
                 * @param data 
                 * @param size 
                 */
            constexpr explicit array_iterator(pointer data, size_t size = 0) noexcept
                : _MyBase(data, size)
            {
            }

            /**
             * @brief Gives acess to element data for a pointer
             * 
             * 
             * @return constexpr reference 
             */
            [[nodiscard]] constexpr reference operator*() const noexcept
            {
                return const_cast<reference>(_MyBase::operator*());
            }

            /**
             * @brief Gives acess to element methods for a pointer
             * 
             * @return constexpr pointer 
             */
            [[nodiscard]] constexpr pointer operator->() const noexcept
            {
                return const_cast<pointer>(_MyBase::operator->());
            }

            /**
             * @brief Add to index
             * 
             * @return constexpr array_const_iterator 
             */
            constexpr array_iterator &operator++() noexcept
            {
                _MyBase::operator++();
                return *this;
            }

            /**
             * @brief Add to index
             * 
             * @return constexpr array_const_iterator 
             */
            constexpr array_iterator operator++(int) noexcept
            {
                array_iterator temp = *this;
                _MyBase::operator++();
                return temp;
            }

            /**
             * @brief Decrements to index
             * 
             * @return constexpr array_const_iterator 
             */
            constexpr array_iterator operator--() noexcept
            {
                _MyBase::operator--();
                return *this;
            }

            /**
             * @brief Decrements to index
             * 
             * @return constexpr array_const_iterator 
             */
            constexpr array_iterator operator--(int) noexcept
            {
                array_iterator temp = *this;
                _MyBase::operator--();
                return temp;
            }

            /**
             * @brief Add to index a especfic off set
             * 
             * @param offset - diference pointer type
             * @return constexpr array_const_iterator 
             */
            constexpr array_iterator operator+=(const ptrdiff_t offset) noexcept
            {
                auto discart = _MyBase::operator+=(offset);
                return *this;
            }

            /**
             * @brief Add to index a especfic off set
             * 
             * @param offset - diference pointer type
             * @return constexpr array_const_iterator 
             */
            [[nodiscard]] constexpr array_iterator operator+(const ptrdiff_t offset) const noexcept
            {
                array_iterator temp = *this;
                return temp += offset;
            }

            /**
             * @brief Decrease to index a especfic off set
             * 
             * @param offset 
             * @return constexpr array_const_iterator 
             */
            constexpr array_iterator operator-=(const ptrdiff_t offset) noexcept
            {
                auto discart = _MyBase::operator-=(offset);
                return *this;
            }

            /**
             * @brief Decrease to index a especfic off set
             * 
             * @param offset 
             * @return constexpr array_const_iterator 
             */
            [[nodiscard]] constexpr array_iterator operator-(const ptrdiff_t offset) noexcept
            {
                array_iterator temp = *this;
                return temp -= offset;
            }

            /**
             * @brief Gives a reference data of the especifc off set
             * 
             * @param offset 
             * @return constexpr reference 
             */
            constexpr reference operator[](const ptrdiff_t offset) const noexcept
            {
                return _MyBase::operator[](offset);
            }

            /**
             * @brief Compare elements
             * 
             * @param rigth 
             * @return true 
             * @return false 
             */
            [[nodiscard]] constexpr bool operator!=(const array_iterator &right) const noexcept
            {
                return _MyBase::operator!=(right);
            }

            /**
             * @brief Compare arrays
             * 
             * @param rigth 
             * @return true 
             * @return false 
             */
            [[nodiscard]] constexpr bool operator==(const array_iterator &rigth) const noexcept
            {
                return _MyBase::operator==(rigth);
            }
        };
    } // namespace array

    template <class T, size_t _size>
    class array
    {
    public:
        // define using
        using const_iterator = iterators::array_const_iterator<T, _size>;
        using iterator = iterators::array_iterator<T, _size>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        using pointer = T *;
        using reference = T &;

        /**
        * @brief Construct a new array object
        * 
        */
        array()
        {
        }

        /**
         * @brief Gets an reference in position of array
         * 
         * @param position 
         * @return constexpr reference 
         */
        [[nodiscard]] constexpr reference at(size_t position)
        {
            _ASSERT_EXPR(position < _size, "cannot seek array before begin");
            return _data[position];
        }

        /**
         * @brief Return an reference from the back of the array
         * 
         * @return constexpr reference 
         */
        [[nodiscard]] constexpr reference back() noexcept
        {
            return _data[_size - 1];
        }

        /**
         * @brief Gives an reference from the front of the array
         * 
         * @return constexpr iterator 
         */
        [[nodiscard]] constexpr iterator begin() noexcept
        {
            return iterator(data(), 0);
        }

        /**
         * @brief Returns an constant iterator from the begin of the array
         * 
         * @return constexpr const_iterator 
         */
        [[nodiscard]] constexpr const_iterator cbegin() const noexcept
        {
            return const_iterator(data(), 0);
        }

        /**
         * @brief Gets pointer to data
         * 
         * @return constexpr pointer 
         */
        [[nodiscard]] constexpr pointer data() noexcept
        {
            return static_cast<T *>(_data);
        }

        /**
         * @brief Alwaya returns false, because the array will never be decreased in size
         * 
         * @return true 
         * @return false 
         */
        [[nodiscard]] constexpr bool empty() const noexcept
        {
            return false;
        }

        /**
         * @brief Returns an constant iterator from the end of the array
         * 
         * @return constexpr const_iterator 
         */
        [[nodiscard]] constexpr const_iterator cend() const noexcept
        {
            return const_iterator(data(), _size);
        }

        /**
         * @brief Gives an reference from the end of the array
         * 
         * @return constexpr iterator 
         */
        [[nodiscard]] constexpr iterator end() noexcept
        {
            return iterator(data(), _size);
        }

        /**
         * @brief Gives an reference from the front of the array
         * 
         * @return constexpr iterator 
         */
        [[nodiscard]] constexpr reference front() noexcept
        {
            return _data[0];
        }

        /**
         * @brief Returns the size of the array
         * 
         * @return constexpr size_t 
         */
        [[nodiscard]] constexpr size_t size() const noexcept
        {
            return _size;
        }

        /**
         * @brief Return a reverse iterator from the end of the array
         * 
         * @return constexpr reverse_iterator 
         */
        [[nodiscard]] constexpr reverse_iterator rbegin() noexcept
        {
            return reverse_iterator(end());
        }

        /**
         * @brief Return a constant reverse iterator from the end of the array
         * 
         * @return constexpr const_reverse_iterator 
         */
        [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept
        {
            return const_reverse_iterator(end());
        }

        /**
         * @brief Return a reverse iterator from the begin of the array
         * 
         * @return constexpr const_reverse_iterator 
         */
        [[nodiscard]] constexpr reverse_iterator rend() noexcept
        {
            return reverse_iterator(begin());
        }

        /**
         * @brief Return a constant reverse iterator from the begin of the array
         * 
         * @return constexpr const_reverse_iterator 
         */
        [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept
        {
            return const_reverse_iterator(begin());
        }

        /**
         * @brief Fills the entire array with the value in paramameter
         * 
         * @param value 
         */
        void fill(T value) noexcept
        {
            for (int i = 0; i < _size; i++)
            {
                if (_data[i] != value)
                    _data[i] = value;
            }
        }

        /**
         * @brief Applies to the range passed in the array, the delegate passed as a parameter
         * 
         * @param delegate 
         * @param starting_position 
         * @param final_position 
         */
        void map(std::function<T(T)> delegate, size_t starting_position = 0, size_t final_position = _size)
        {
            _ASSERT_EXPR(starting_position < _size, "canot seek array after and");

            for (int i = starting_position; i < final_position; i++)
            {
                _data[i] = delegate(_data[i]);
            }
        }

        /**
         * @brief Gets an reference in position of array
         * 
         * @param position 
         * @return constexpr reference 
         */
        [[nodiscard]] constexpr T &operator[](size_t position) noexcept
        {
            _ASSERT_EXPR(position < _size, "canot seek array after and");
            return _data[position];
        }

        /**
         * @brief Gets an reference in position of array
         * 
         * @param position 
         * @return constexpr reference 
         */
        [[nodiscard]] constexpr T &operator[](size_t position) const noexcept
        {
            _ASSERT_EXPR(position < _size, "canot seek array after and");
            return _data[position];
        }

    private:
        T _data[_size];
    };
}
