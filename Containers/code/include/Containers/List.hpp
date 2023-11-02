/**
 * @file List.hpp
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

#include "Storage.hpp"
#include <optional>
#include <exception>

namespace eho {
    template<typename t_tType>
    class IListView {
    protected:
        using ConstIterator = Internal::CIterator<const t_tType>;

    public:

        virtual const t_tType &at(size_t uIndex) const = 0;

        virtual const t_tType &operator[](size_t uIndex) const = 0;

        virtual size_t size() const = 0;

        virtual bool empty() const = 0;

        virtual ConstIterator begin() const = 0;

        virtual ConstIterator end() const = 0;
    };

    template<typename t_tType, size_t t_uSize, bool t_bLinked, bool t_bAmortized>
    class CBaseListImplementation : IListView<t_tType> {
    protected:
        using ConstIterator = IListView<t_tType>::ConstIterator;
        using Iterator = Internal::CIterator<t_tType>;

    public:
        const t_tType &at(size_t uIndex) const override {
            return InnerAt(uIndex);
        }

        virtual t_tType &at(size_t uIndex) {
            return InnerAt(uIndex);
        }

        const t_tType &operator[](size_t uIndex) const override {
            return InnerAt(uIndex);
        }

        virtual t_tType &operator[](size_t uIndex) {
            return InnerAt(uIndex);
        }

        size_t size() const override {
            return m_Storage.size();
        }

        bool empty() const override {
            return m_Storage.size() == 0;
        }

        t_tType *data() { return m_Storage.data(); }

        const t_tType *data() const { return m_Storage.data(); }

        Iterator begin() {
            return m_Storage.begin();
        }

        virtual Iterator end() {
            return m_Storage.end();
        }

        ConstIterator begin() const override {
            return m_Storage.begin();
        }

        ConstIterator end() const override {
            return m_Storage.end();
        }

    protected:
        Internal::CContainer<t_tType, t_uSize, t_bLinked, t_bAmortized> m_Storage;

        virtual inline t_tType &InnerAt(size_t uIndex) {
            if (uIndex >= m_Storage.size()) {
                throw std::out_of_range{"Requested index is out of range"};
            }

            return m_Storage[uIndex];
        }

        virtual inline const t_tType &InnerAt(size_t uIndex) const {
            if (uIndex >= m_Storage.size()) {
                throw std::out_of_range{"Requested index is out of range"};
            }

            return m_Storage[uIndex];
        }
    };

    template<typename t_tType, bool t_bLinked, bool t_bAmortized>
    class CDynamicListImplementation : public CBaseListImplementation<t_tType, 0, t_bLinked, t_bAmortized> {
    protected:
        using Base = CBaseListImplementation<t_tType, 0, t_bLinked, t_bAmortized>;

    public:
        size_t size() const override {
            return m_uUsedSize;
        }

        size_t capacity() const {
            return Base::m_Storage.size();
        }

        bool empty() const override {
            return m_uUsedSize == 0;
        }

        /**
         * Will resize the container to uNewSize.
         * <br/><br/>
         * If uNewSize = capacity(): It will do nothing.
         * <br/><br/>
         * If uNewSize > capacity(): It will reserve the memory,
         * size() will not be affected. Use insert() to update the values after size()-1.
         * Will do nothing if t_bAmortized=false.
         * <br/><br/>
         * If uNewSize < capacity(): size() will become uNewSize, the last elements
         * of the list will be invalidated. Be aware that if t_bAmortized=true,
         * capacity() will be kept as it is.
         * @param uNewSize The container's new capacity.
         */
        void resize(size_t uNewSize) {
            auto uUpdatedSize = Base::m_Storage.resize(uNewSize);
            if (uNewSize < Base::m_Storage.size()) {
                m_uUsedSize = uUpdatedSize;
            }
        }

        void clear() {
            m_uUsedSize = Base::m_Storage.resize(0);
        }

        void insert(const t_tType &Item) {
            this->insert(m_uUsedSize, Item);
        }

        void insert(const t_tType &&Item) {
            this->insert(m_uUsedSize, Item);
        }

        void insert(size_t uIndex, const t_tType &Item) {
            Base::m_Storage.insert(Item, uIndex, m_uUsedSize);
            m_uUsedSize += 1;
        }

        void insert(size_t uIndex, t_tType &&Item) {
            Base::m_Storage.insert(Item, uIndex, m_uUsedSize);
            m_uUsedSize += 1;
        }

        std::optional<t_tType> pop() {
            size_t uIndex = m_uUsedSize == 0 ? 0 : m_uUsedSize - 1;
            return this->pop(uIndex);
        }

        std::optional<t_tType> pop(size_t uIndex) {
            std::optional<t_tType> RtnVal{};
            if (uIndex < m_uUsedSize) {
                RtnVal.emplace(std::move(Base::m_Storage.pop(uIndex, m_uUsedSize)));
                m_uUsedSize -= 1;
            }

            return RtnVal;
        }

        Base::Iterator end() override {
            return Base::m_Storage.begin() + m_uUsedSize;
        }

        Base::ConstIterator end() const override {
            return Base::m_Storage.begin() + m_uUsedSize;
        }

    protected:
        size_t m_uUsedSize = 0;

        inline t_tType &InnerAt(size_t uIndex) override {
            if (uIndex >= m_uUsedSize) {
                throw std::out_of_range{"Requested index is out of range"};
            }

            return Base::m_Storage[uIndex];
        }

        inline const t_tType &InnerAt(size_t uIndex) const override {
            if (uIndex >= m_uUsedSize) {
                throw std::out_of_range{"Requested index is out of range"};
            }

            return Base::m_Storage[uIndex];
        }
    };

    /**
     * Dynamic allocated list.
     */
    template<typename t_tType, bool t_bAmortized = false>
    using CList = CDynamicListImplementation<t_tType, false, t_bAmortized>;

    /**
     * Static allocated list.
     */
    template<typename t_tType, size_t t_uSize>
    using CListStatic = CBaseListImplementation<t_tType, t_uSize, false, false>;

    /**
     * Dynamic allocated linked list.
     */
    template<typename t_tType, bool t_bAmortized = false>
    using CListLinked = CDynamicListImplementation<t_tType, true, t_bAmortized>;

    /**
     * Static asserts for the lists' iterators
     */
    // Static array
    static_assert(std::ranges::contiguous_range<CListStatic<int, 15>>);
    // Dynamic array
    static_assert(std::ranges::contiguous_range<CList<int>>);
    // Dynamic amortized array
    static_assert(std::ranges::contiguous_range<CList<int, true>>);
    // Linked list
//    static_assert(std::ranges::contiguous_range<CListLinked<int>>);
    // Linked list amortized
//    static_assert(std::ranges::contiguous_range<CListLinked<int, true>>);
}
