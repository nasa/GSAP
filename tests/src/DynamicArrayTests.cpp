// Copyright (c) 2018-2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "DynamicArray.h"
#include "MockClasses.h"
#include "Test.h"

// TODO (JW): Total bytes allocated by std::vector is not predictable or
//            consistent between platforms. Need to replace std::vector in the
//            implementation of DynamicArray and uncomment allocation asserts
//            in these tests.

using namespace PCOE;
using namespace PCOE::Test;

namespace DynamicArrayTests {
    using test_type = double;

    void construct_empty() {
        TestAllocator<test_type> alloc;
        DynamicArray<test_type, TestAllocator<test_type>> arr;
        // Assert::AreEqual(8, *alloc.totalAllocated, "Allocation size");
    }

    void construct() {
        TestAllocator<test_type> alloc;
        std::size_t size = 4;
        DynamicArray<test_type, TestAllocator<test_type>> arr(size, alloc);
        // Assert::AreEqual(8 + size * sizeof(test_type), *alloc.totalAllocated, "Allocation size");
    }

    void construct_copy() {
        TestAllocator<test_type> alloc;
        std::size_t size = 4;

        DynamicArray<test_type, TestAllocator<test_type>> arr0(size, alloc);
        DynamicArray<test_type, TestAllocator<test_type>> arr1(arr0);
        // Assert::AreEqual(2 * (8 + size * sizeof(test_type)), *alloc.totalAllocated, "Allocation
        // size");
    }

    void construct_move() {
        TestAllocator<test_type> alloc;
        std::size_t size = 4;

        DynamicArray<test_type, TestAllocator<test_type>> arr0(size, alloc);
        DynamicArray<test_type, TestAllocator<test_type>> arr1(std::move(arr0));
        // Assert::AreEqual(16 + size * sizeof(test_type), *alloc.totalAllocated, "Allocation
        // size");
    }

    void at() {
        TestAllocator<test_type> alloc;
        std::size_t size = 4;
        DynamicArray<test_type, TestAllocator<test_type>> arr(size, alloc);

        Assert::AreEqual(0.0, arr.at(0), "Default value");
        arr.at(0) = 1.0;
        Assert::AreEqual(1.0, arr.at(0), "Indexer");
        // Assert::AreEqual(8 + size * sizeof(test_type), *alloc.totalAllocated, "Allocation size");
    }

    void index() {
        TestAllocator<test_type> alloc;
        std::size_t size = 4;
        DynamicArray<test_type, TestAllocator<test_type>> arr(size, alloc);

        Assert::AreEqual(0.0, arr[0], "Default value");
        arr[0] = 1.0;
        Assert::AreEqual(1.0, arr[0], "Indexer");
        // Assert::AreEqual(8 + size * sizeof(test_type), *alloc.totalAllocated, "Allocation size");
    }

    void front() {
        TestAllocator<test_type> alloc;
        std::size_t size = 4;
        DynamicArray<test_type, TestAllocator<test_type>> arr(size, alloc);

        Assert::AreEqual(0.0, arr.at(0), "Default value");
        arr[0] = 1.0;
        arr[1] = 2.0;
        arr[2] = 3.0;
        arr[3] = 4.0;
        Assert::AreEqual(1.0, arr.front(), "front");
        // Assert::AreEqual(8 + size * sizeof(test_type), *alloc.totalAllocated, "Allocation size");

        arr.front() = 5.0;
        Assert::AreEqual(5.0, arr.front(), "front 2");
    }

    void back() {
        TestAllocator<test_type> alloc;
        std::size_t size = 4;
        DynamicArray<test_type, TestAllocator<test_type>> arr(size, alloc);

        Assert::AreEqual(0.0, arr.at(0), "Default value");
        arr[0] = 1.0;
        arr[1] = 2.0;
        arr[2] = 3.0;
        arr[3] = 4.0;
        Assert::AreEqual(4.0, arr.back(), "back");
        // Assert::AreEqual(8 + size * sizeof(test_type), *alloc.totalAllocated, "Allocation size");

        arr.back() = 5.0;
        Assert::AreEqual(5.0, arr.back(), "back 2");
    }

    void empty() {
        TestAllocator<test_type> alloc;
        std::size_t size = 4;
        DynamicArray<test_type, TestAllocator<test_type>> arr(0, alloc);
        DynamicArray<test_type, TestAllocator<test_type>> arr1(size, alloc);

        Assert::IsTrue(arr.empty(), "Empty");
        Assert::IsFalse(arr1.empty(), "Not empty");
    }

    void size() {
        TestAllocator<test_type> alloc;
        std::size_t size = 4;
        DynamicArray<test_type, TestAllocator<test_type>> arr(0, alloc);
        DynamicArray<test_type, TestAllocator<test_type>> arr1(size, alloc);

        Assert::AreEqual(0, arr.size(), "Empty");
        Assert::AreEqual(size, arr1.size(), "Not empty");
    }

    void registerTests(TestContext& context) {
        context.AddTest("construct_empty", DynamicArrayTests::construct_empty, "Dynamic Array");
        context.AddTest("construct", DynamicArrayTests::construct, "Dynamic Array");
        context.AddTest("construct_copy", DynamicArrayTests::construct_copy, "Dynamic Array");
        context.AddTest("construct_move", DynamicArrayTests::construct_move, "Dynamic Array");
        context.AddTest("at", DynamicArrayTests::at, "Dynamic Array");
        context.AddTest("index", DynamicArrayTests::index, "Dynamic Array");
        context.AddTest("front", DynamicArrayTests::front, "Dynamic Array");
        context.AddTest("back", DynamicArrayTests::back, "Dynamic Array");
    }
}
