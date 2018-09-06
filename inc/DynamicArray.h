// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_DYNAMICARRAY_H
#define PCOE_DYNAMICARRAY_H

#include <algorithm>
#include <initializer_list>
#include <vector>

namespace PCOE {
    /**
     * An object that contains a sequence of elements of type T. Unlike
     * {@code std::array}, the size of {@code Dynamic} array is determined when
     * the dynamic array is created.
     *
     * This class is essentially a wrapper around a {@code std::vector} that
     * provides the same interface as {@code std::array}.
     *
     * @paramt T The type of the elements contained by the array.
     *
     * @author Jason Watkins
     * @since 1.2
     **/
    template <class T, class Allocator = std::allocator<T>>
    class DynamicArray {
    public:
        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using iterator = typename std::vector<T, Allocator>::iterator;
        using const_iterator = typename std::vector<T, Allocator>::const_iterator;
        using reverse_iterator = typename std::vector<T, Allocator>::reverse_iterator;
        using const_reverse_iterator = typename std::vector<T, Allocator>::const_reverse_iterator;

        /**
         * Constructs an empty {@code DynamicArray}.
         **/
        DynamicArray() = default;

        /**
         * Constructs a new {@code DynamicArray}, copying size and contents of
         * {@param other}.
         *
         * @param other The array to copy from.
         **/
        DynamicArray(const DynamicArray& other) = default;

        /**
         * Constructs a new {@code DynamicArray}, moving the contents of
         * {@param other} into the new instance.
         *
         * @param other The array to move from.
         **/
        DynamicArray(DynamicArray&& other) = default;

        /**
         * Constructs a new {@code DynamicArray} with the specified number of
         * elements.
         *
         * @param size  The size of the new array.
         * @param alloc The allocator used to get memory for the underlying
         *              storage. If no allocator is provided, the default
         *              constructed allocator is used.
         **/
        explicit DynamicArray(size_type size, const Allocator& alloc = Allocator())
            : storage(alloc) {
            storage.resize(size);
        }

        /**
         * Constructs a new {@code DynamicArray}, copying size and contents of
         * {@param source}.
         *
         * @param source The array to copy from.
         **/
        explicit DynamicArray(std::initializer_list<T> source, const Allocator& alloc = Allocator())
            : storage(source, alloc) {}

        /**
         * Constructs a new {@code DynamicArray}, copying size and contents of
         * {@param source}.
         *
         * @param source The vector to copy from.
         * @param alloc The allocator used to get memory for the underlying
         *              storage. If no allocator is provided, the default
         *              constructed allocator is used.
         **/
        explicit DynamicArray(const std::vector<T, Allocator>& source,
                              const Allocator& alloc = Allocator())
            : storage(source, alloc) {}

        /**
         * Constructs a new {@code DynamicArray}, moving the contents of
         * {@param source}.
         *
         * @param source The vector to move from.
         **/
        explicit DynamicArray(std::vector<T, Allocator>&& source) : storage(source) {}

        /**
         * Virtual destructor allows derived classes to be safely destroyed via
         * a pointer to the base type.
         **/
        virtual ~DynamicArray() = default;

        /**
         * Replaces the contents of the current instance with the contents of
         * {@param other}.
         *
         * @param other Contains the contents to assign to the current instance.
         * @return      A reference to the current instance.
         **/
        DynamicArray& operator=(DynamicArray other) {
            this->swap(other);
            return *this;
        }

        /**
         * Swaps the contents of the current instance with the contents of
         * {@param other}.
         *
         * @param other Contains the contents to assign to the current instance.
         **/
        void swap(DynamicArray& other) noexcept {
            storage.swap(other.storage);
        }

        /**
         * Swaps the contents of {@param first} with the contents of
         * {@param second}.
         *
         * @param first The first instance.
         * @param second The second instance.
         **/
        friend void swap(DynamicArray& first, DynamicArray& second) {
            using std::swap;

            swap(first.storage, second.storage);
        }

        /**
         * Checks whether the given arrays are the same size and contain the
         * same elements.
         **/
        friend bool operator==(const DynamicArray& lhs, const DynamicArray& rhs) {
            return lhs.storage == rhs.storage;
        }

        /**
         * Checks whether the given arrays differ in size or elements conatined.
         **/
        friend bool operator!=(const DynamicArray& lhs, const DynamicArray& rhs) {
            return lhs.storage != rhs.storage;
        }

        /**
         * Returns a reference to the element at the given position. This method
         * executes a bounds check before getting the value and throws and
         * exception if the specified index is out of bounds.
         *
         * @param pos The index of the value to get a reference to.
         * @return    A reference to the element at the given position.
         * @exception std::out_of_range If {@param pos} is out of range.
         **/
        reference at(size_type pos) {
            return storage.at(pos);
        }

        /**
         * Returns a const reference to the element at the given position. This
         * method executes a bounds check before getting the value and throws
         * and exception if the specified index is out of bounds.
         *
         * @param pos The index of the value to get a reference to.
         * @return    A cosnt reference to the element at the given position.
         * @exception std::out_of_range If {@param pos} is out of range.
         **/
        const_reference at(size_type pos) const {
            return storage.at(pos);
        }

        /**
         * Returns a reference to the element at the given position. The index
         * operator does not do a bounds check.
         *
         * @param pos The index of the value to get a reference to.
         * @return    A reference to the element at the given position.
         **/
        reference operator[](size_type pos) {
            return storage[pos];
        }

        /**
         * Returns a const reference to the element at the given position. The
         * index operator does not do a bounds check.
         *
         * @param pos The index of the value to get a reference to.
         * @return    A cosnt reference to the element at the given position.
         **/
        const_reference operator[](size_type pos) const {
            return storage[pos];
        }
        
        /**
         * Returns a reference to the first element of the array. The behavior
         * of this method is undefined when the array has size 0.
         *
         * @return A reference to the first element of the array.
         **/
        reference front() {
            return storage.front();
        }

        /**
         * Returns a const reference to the first element of the array. The
         * behavior of this method is undefined when the array has size 0.
         *
         * @return A const reference to the first element of the array.
         **/
        const_reference front() const {
            return storage.front();
        }

        /**
         * Returns a reference to the last element of the array. The behavior
         * of this method is undefined when the array has size 0.
         *
         * @return A reference to the last element of the array.
         **/
        reference back() {
            return storage.back();
        }

        /**
         * Returns a const reference to the last element of the array. The
         * behavior of this method is undefined when the array has size 0.
         *
         * @return A const reference to the last element of the array.
         **/
        const_reference back() const {
            return storage.back();
        }

        /**
         * Gets a pointer to the underlying storage of the array.
         **/
        pointer data() noexcept {
            return storage.data();
        }

        /**
         * Gets a pointer to the underlying storage of the array.
         **/
        const_pointer data() const noexcept {
            return storage.data();
        }

        /**
         * Gets a reference to the underlying vector of the array.
         **/
        const std::vector<T>& vec() const {
            return storage;
        }

        /**
         * Returns an iterator to the first element of the array.
         **/
        iterator begin() {
            return storage.begin();
        }

        /**
         * Returns an iterator one past the last element of the array.
         **/
        iterator end() {
            return storage.end();
        }

        /**
         * Returns an constant iterator to the first element of the array.
         **/
        const_iterator cbegin() const {
            return storage.cbegin();
        }

        /**
         * Returns an constant iterator one past the last element of the array.
         **/
        const_iterator cend() const {
            return storage.cend();
        }

        /**
         * Returns an iterator to the first element of the array.
         **/
        iterator rbegin() {
            return storage.begin();
        }

        /**
         * Returns an iterator one past the last element of the array.
         **/
        iterator rend() {
            return storage.end();
        }
        
        /**
         * Elementwise addition
         */
        inline DynamicArray& operator+=(DynamicArray& toAdd) {
            for (size_type i = 0; i < len; ++i) {
                this->data[i] += toAdd[i];
            }
            return *this;
        }
        
        inline friend DynamicArray operator+(DynamicArray lhs, const DynamicArray& rhs){
            return lhs += rhs;
        }
        
        
        
        /**
         * Elementwise subtraction
         */
        inline DynamicArray& operator-=(DynamicArray& toSubtract) {
            for (size_type i = 0; i < len; ++i) {
                this->data[i] -= toSubtract[i];
            }
            return *this;
        }
        
        inline friend DynamicArray operator-(DynamicArray lhs, const DynamicArray& rhs){
            return lhs -= rhs;
        }
        


        /**
         * Returns an constant iterator to the first element of the array.
         **/
        const_iterator crbegin() const {
            return storage.cbegin();
        }

        /**
         * Returns an constant iterator one past the last element of the array.
         **/
        const_iterator crend() const {
            return storage.cend();
        }

        /**
         * Returns a value indicating whether the array has no elements.
         **/
        bool empty() const noexcept {
            return storage.empty();
        }

        /**
         * Returns the number of elements in the array.
         **/
        size_type size() const {
            return storage.size();
        }

        /**
         * Copies the provided value to all elements of the array.
         **/
        void fill(const T& value) noexcept {
            std::fill(storage.begin(), storage.end(), value);
        }

    private:
        std::vector<T, Allocator> storage;
    };
}
#endif
