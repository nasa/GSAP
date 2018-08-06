// Copyright (c) 2017-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_DYNAMICARRAY_H
#define PCOE_DYNAMICARRAY_H

#include <algorithm>
#include <initializer_list>
#include <vector>

namespace PCOE {
    template <class T>
    class DynamicArray {
    public:
        using size_type = std::size_t;
        using value_type = T;
        using reference = T&;
        using const_reference = const T&;
        using iterator = T*;
        using const_iterator = T* const;

        DynamicArray() : data(nullptr), len(0) {}

        DynamicArray(const DynamicArray& other)
            : len(other.len), data(other.data == nullptr ? nullptr : new T[other.len]) {
            std::copy(other.data, other.data + other.len, data);
        }

        DynamicArray(DynamicArray&& other) : DynamicArray() {
            swap(*this, other);
        }

        explicit DynamicArray(size_type size) {
            len = size;

            data = new T[size];
        }

        explicit DynamicArray(std::initializer_list<T> source) : DynamicArray(source.size()) {
            std::copy(source.begin(), source.end(), data);
        }

        explicit DynamicArray(const std::vector<T>& source) : DynamicArray(source.size()) {
            for (size_type i = 0; i < len; ++i) {
                data[i] = source[i];
            }
        }

        virtual ~DynamicArray() {
            delete[] data;
        }

        DynamicArray& operator=(DynamicArray other) {
            swap(*this, other);
            return *this;
        }

        friend void swap(DynamicArray& first, DynamicArray& second) {
            using std::swap;

            swap(first.data, second.data);
            swap(first.len, second.len);
        }

        reference operator[](size_type i) {
            return data[i];
        }

        const_reference operator[](size_type i) const {
            return data[i];
        }

        iterator begin() {
            return data;
        }

        iterator end() {
            return data + len;
        }

        const_iterator cbegin() {
            return data;
        }

        const_iterator cend() {
            return data + len;
        }

        size_type size() const {
            return len;
        }

        std::vector<T> vec() const {
            std::vector<T> result(len);
            for (size_type i = 0; i < len; ++i) {
                result[i] = data[i];
            }
            return result;
        }

    private:
        T* data;
        size_type len;
    };

    template <class T>
    bool operator==(const DynamicArray<T>& lhs, const DynamicArray<T>& rhs) {
        if (lhs.size() != rhs.size()) {
            return false;
        }
        for (typename DynamicArray<T>::size_type i = 0; i < lhs.size(); ++i) {
            if (lhs[i] != rhs[i]) {
                return false;
            }
        }
        return true;
    }

    template <class T>
    bool operator!=(const DynamicArray<T>& lhs, const DynamicArray<T>& rhs) {
        return !(lhs == rhs);
    }
}
#endif
