/**
 * @file Storage.hpp
 * @brief
 * @version 0.0.1
 * @date 2023-01-21
 *
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 ********************************************************************************/

#pragma once

#include <iterator>
#include <memory>
#include <functional>

namespace eho::Internal {
    template<typename t_tType>
    class CIterator {
        // thanks: https://stackoverflow.com/questions/69890176/create-contiguous-iterator-for-custom-class
    public:
        using iterator_category = std::contiguous_iterator_tag;
        using iterator_concept = std::contiguous_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = std::remove_cv_t<t_tType>;
        using pointer = t_tType *;
        using reference = t_tType &;

        // constructor for Array<T,S>::begin() and Array<T,S>::end()
        CIterator(pointer ptr) : m_Ptr(ptr) {}

        // std::weakly_incrementable<I>
        CIterator &operator++() {
            ++m_Ptr;
            return *this;
        }

        CIterator operator++(int) {
            CIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        CIterator() : m_Ptr(nullptr/*&mArray[0]*/) {} // TODO: Unsure which is correct!

        // std::input_or_output_iterator<I>
        reference operator*() { return *m_Ptr; }

        // std::indirectly_readable<I>
        friend reference operator*(const CIterator &it) { return *(it.m_Ptr); }

        // std::input_iterator<I>
        // No actions were needed here!

        // std::forward_iterator<I>
        // In C++20, 'operator==' implies 'operator!='
        bool operator==(const CIterator &it) const { return m_Ptr == it.m_Ptr; }

        // std::bidirectional_iterator<I>
        CIterator &operator--() {
            --m_Ptr;
            return *this;
        }

        CIterator operator--(int) {
            CIterator tmp = *this;
            --(*this);
            return tmp;
        }

        // std::random_access_iterator<I>
        //     std::totally_ordered<I>
        std::weak_ordering operator<=>(const CIterator &it) const {
            return std::compare_three_way{}(m_Ptr, it.m_Ptr);
            // alternatively: `return mPtr <=> it.mPtr;`
        }

        //     std::sized_sentinel_for<I, I>
        difference_type operator-(const CIterator &it) const { return m_Ptr - it.m_Ptr; }

        //     std::iter_difference<I> operators
        CIterator &operator+=(difference_type diff) {
            m_Ptr += diff;
            return *this;
        }

        CIterator &operator-=(difference_type diff) {
            m_Ptr -= diff;
            return *this;
        }

        CIterator operator+(difference_type diff) const { return CIterator(m_Ptr + diff); }

        CIterator operator-(difference_type diff) const { return CIterator(m_Ptr - diff); }

        friend CIterator operator+(difference_type diff, const CIterator &it) {
            return it + diff;
        }

        friend CIterator operator-(difference_type diff, const CIterator &it) {
            return it - diff;
        }

        reference operator[](difference_type diff) const { return m_Ptr[diff]; }

        // std::contiguous_iterator<I>
        pointer operator->() const { return m_Ptr; }

        using element_type = t_tType;

    private:
        t_tType *m_Ptr;
    };

    template<typename t_tType, size_t t_uSize, bool t_bLinked, bool t_bAmortized>
    class CContainer;

    /**
     * Static size container implementation.
     * It is allocated in contiguous memory.
     * @tparam t_tType Container's data type.
     * @tparam t_uSize Container's size.
     */
    template<typename t_tType, size_t t_uSize> requires(t_uSize > 0)
    class CContainer<t_tType, t_uSize, false, false> {
    public:
        using Iterator = CIterator<t_tType>;
        //https://stackoverflow.com/questions/3582608/how-to-correctly-implement-custom-iterators-and-const-iterators
        using ConstIterator = CIterator<const t_tType>;

    public:
        CContainer() = default;

        inline t_tType &operator[](size_t uIndex) { return m_arStorage[uIndex]; }

        inline const t_tType &operator[](size_t uIndex) const { return m_arStorage[uIndex]; }

        inline constexpr size_t size() const { return t_uSize; }

        inline t_tType *data() { return m_arStorage; }

        inline const t_tType *data() const { return m_arStorage; }

        Iterator begin() { return Iterator(&m_arStorage[0]); }

        Iterator end() { return Iterator(&m_arStorage[t_uSize]); }

        ConstIterator begin() const { return ConstIterator(&m_arStorage[0]); }

        ConstIterator end() const { return ConstIterator(&m_arStorage[t_uSize]); }

        ConstIterator cbegin() const { return begin(); }

        ConstIterator cend() const { return end(); }

    protected:
        t_tType m_arStorage[t_uSize];

    private:
        // === STATIC ASSERTS - to verify correct Iterator implementation! ===
        static_assert(std::weakly_incrementable<Iterator>);
        static_assert(std::input_or_output_iterator<Iterator>);
        static_assert(std::indirectly_readable<Iterator>);
        static_assert(std::input_iterator<Iterator>);
        static_assert(std::incrementable<Iterator>);
        static_assert(std::forward_iterator<Iterator>);
        static_assert(std::bidirectional_iterator<Iterator>);
        static_assert(std::totally_ordered<Iterator>);
        static_assert(std::sized_sentinel_for<Iterator, Iterator>);
        static_assert(std::random_access_iterator<Iterator>);
        static_assert(std::is_lvalue_reference_v<std::iter_reference_t<Iterator>>);
        static_assert(
                std::same_as<std::iter_value_t<Iterator>, std::remove_cvref_t<std::iter_reference_t<Iterator>>>);
        static_assert(std::contiguous_iterator<Iterator>);
        // === STATIC ASSERTS - to verify correct ConstIterator implementation! ===
        // https://stackoverflow.com/questions/29829810/test-for-const-behavior-with-googletest
        static_assert(std::is_const<typename std::remove_reference<decltype(*ConstIterator())>::type>::value);
        static_assert(std::weakly_incrementable<ConstIterator>);
        static_assert(std::input_or_output_iterator<ConstIterator>);
        static_assert(std::indirectly_readable<ConstIterator>);
        static_assert(std::input_iterator<ConstIterator>);
        static_assert(std::incrementable<ConstIterator>);
        static_assert(std::forward_iterator<ConstIterator>);
        static_assert(std::bidirectional_iterator<ConstIterator>);
        static_assert(std::totally_ordered<ConstIterator>);
        static_assert(std::sized_sentinel_for<ConstIterator, ConstIterator>);
        static_assert(std::random_access_iterator<ConstIterator>);
        static_assert(std::is_lvalue_reference_v<std::iter_reference_t<ConstIterator>>);
        static_assert(
                std::same_as<std::iter_value_t<Iterator>, std::remove_cvref_t<std::iter_reference_t<ConstIterator>>>);
        static_assert(std::contiguous_iterator<ConstIterator>);
    };

    /**
     * Dynamic sized container implementation.
     * It is allocated in contiguous memory.
     * @tparam t_tType
     * @tparam t_bAmortized
     */
    template<typename t_tType, bool t_bAmortized>
    class CContainer<t_tType, 0, false, t_bAmortized> {
    public:
        using Iterator = CIterator<t_tType>;
        using ConstIterator = CIterator<const t_tType>;

    public:
        CContainer() : m_uSize{0}, m_uInitSize{0}, m_Storage{nullptr, [](t_tType *ptr) {}} {}

        ~CContainer() {
            for (size_t i = 0; i < m_uInitSize; ++i) {
                std::allocator_traits<decltype(m_Allocator)>::destroy(m_Allocator, &m_Storage[i]);
            }
        }

        inline t_tType &operator[](size_t uIndex) { return m_Storage[uIndex]; }

        inline const t_tType &operator[](size_t uIndex) const { return m_Storage[uIndex]; }

        inline size_t size() const { return m_uSize; }

        size_t resize(size_t uNewSize) {
            size_t uSize = 0;
            if constexpr (t_bAmortized) {
                uSize = std::max(uNewSize + (uNewSize / 2), m_uSize);

                if (uSize > uNewSize) {
                    // We will not deallocate the memory region, but we need to destroy the objects
                    std::ranges::destroy(this->begin() + uSize, this->end());
                }

            } else {
                uSize = uNewSize;
            }

            if (uSize == 0) {
                m_Storage.reset(nullptr);
                m_uSize = 0;
            } else if (uSize != m_uSize) {
                std::unique_ptr<t_tType[], std::function<void(t_tType *)>> NewStorage{
                        m_Allocator.allocate(uSize),
                        std::bind([](t_tType *ptr, std::allocator<t_tType> Allocator, size_t uSize) {
                            for (size_t i = 0; i < uSize; ++i) {
//                                std::allocator_traits<decltype(Allocator)>::destroy(Allocator, ptr + i);
                            }
                            std::allocator_traits<decltype(Allocator)>::deallocate(Allocator, ptr, uSize);
                        }, std::placeholders::_1, m_Allocator, uSize)};

                if (m_uSize != 0) {
                    // Copy one to another
                    std::ranges::uninitialized_move(Iterator(&m_Storage[0]), Iterator(&m_Storage[m_uSize]),
                                                    Iterator(&NewStorage[0]), Iterator(&NewStorage[uSize]));
                }

                m_Storage.swap(NewStorage);
                m_uSize = uSize;
            } else {
                uSize = m_uSize;
            }
            return uSize;
        }

        inline t_tType *data() { return m_Storage.get(); }

        inline const t_tType *data() const { return m_Storage.get(); }

        void insert(t_tType &&Item, size_t uIndex, size_t uShift) {
            m_uInitSize += 1;
            AllocateAndShift(uIndex, uShift);
            std::construct_at(&m_Storage[uIndex], std::move(Item));
        }

        void insert(const t_tType &Item, size_t uIndex, size_t uShift) {
            m_uInitSize += 1;
            AllocateAndShift(uIndex, uShift);
            std::construct_at(&m_Storage[uIndex], Item);
        }

        t_tType pop(size_t uIndex, size_t uShift) {
            t_tType RtnVal{std::move(m_Storage[uIndex])};
            if (uIndex < uShift) {
                std::ranges::move(begin() + uIndex + 1, end(), begin() + uIndex);
            }
            resize((uShift - 1));
            return RtnVal;
        }

        Iterator begin() { return Iterator(&m_Storage[0]); }

        Iterator end() { return Iterator(&m_Storage[m_uSize]); }

        ConstIterator begin() const { return ConstIterator(&m_Storage[0]); }

        ConstIterator end() const { return ConstIterator(&m_Storage[m_uSize]); }

        ConstIterator cbegin() const { return begin(); }

        ConstIterator cend() const { return end(); }

    protected:
        size_t m_uSize;
        size_t m_uInitSize;
        std::allocator<t_tType> m_Allocator;
        std::unique_ptr<t_tType[], std::function<void(t_tType *)>> m_Storage;

        inline void AllocateAndShift(size_t uIndex, size_t uShift) {
            if ((uShift + 1) > m_uSize) {
                resize((uShift + 1));
            }

            if (uIndex < uShift) {
                for (auto it = begin() + uShift; it != (begin() + uIndex); --it) {
                    std::uninitialized_move(it - 1, it, it);
                }
            }
        }


        template<typename T>
        std::unique_ptr<T[], std::function<void(T *)>> make_T(t_tType *ptr, std::allocator<T> alloc, std::size_t size) {
            auto deleter = [](T *p, std::allocator<T> alloc, std::size_t size) {
                for (size_t i = 0; i < size; ++i) {
//                    alloc.destroy(&p[i]);
                }
                alloc.deallocate(p, size);
            };

            return {ptr, std::bind(deleter, std::placeholders::_1, alloc, size)};
        }

    private:
        // === STATIC ASSERTS - to verify correct Iterator implementation! ===
        static_assert(std::weakly_incrementable<Iterator>);
        static_assert(std::input_or_output_iterator<Iterator>);
        static_assert(std::indirectly_readable<Iterator>);
        static_assert(std::input_iterator<Iterator>);
        static_assert(std::incrementable<Iterator>);
        static_assert(std::forward_iterator<Iterator>);
        static_assert(std::bidirectional_iterator<Iterator>);
        static_assert(std::totally_ordered<Iterator>);
        static_assert(std::sized_sentinel_for<Iterator, Iterator>);
        static_assert(std::random_access_iterator<Iterator>);
        static_assert(std::is_lvalue_reference_v<std::iter_reference_t<Iterator>>);
        static_assert(
                std::same_as<std::iter_value_t<Iterator>, std::remove_cvref_t<std::iter_reference_t<Iterator>>>);
        static_assert(std::contiguous_iterator<Iterator>);
        // === STATIC ASSERTS - to verify correct ConstIterator implementation! ===
        // https://stackoverflow.com/questions/29829810/test-for-const-behavior-with-googletest
        static_assert(std::is_const<typename std::remove_reference<decltype(*ConstIterator())>::type>::value);
        static_assert(std::weakly_incrementable<ConstIterator>);
        static_assert(std::input_or_output_iterator<ConstIterator>);
        static_assert(std::indirectly_readable<ConstIterator>);
        static_assert(std::input_iterator<ConstIterator>);
        static_assert(std::incrementable<ConstIterator>);
        static_assert(std::forward_iterator<ConstIterator>);
        static_assert(std::bidirectional_iterator<ConstIterator>);
        static_assert(std::totally_ordered<ConstIterator>);
        static_assert(std::sized_sentinel_for<ConstIterator, ConstIterator>);
        static_assert(std::random_access_iterator<ConstIterator>);
        static_assert(std::is_lvalue_reference_v<std::iter_reference_t<ConstIterator>>);
        static_assert(
                std::same_as<std::iter_value_t<Iterator>, std::remove_cvref_t<std::iter_reference_t<ConstIterator>>>);
        static_assert(std::contiguous_iterator<ConstIterator>);
    };

    template<typename t_tType, bool t_bAmortized>
    class CContainer<t_tType, 0, true, t_bAmortized> {
    };
}
