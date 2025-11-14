/**
 * Test file for grid3d memory leak fixes
 * Tests Critical Issue 1.2: Missing destructor implementation
 */

#include <iostream>
#include <cassert>
#include <cstring>

// Include the grid3d header
#include "../base/mqi_grid3d.hpp"

// Simple memory tracking (for demonstration)
static size_t allocation_count = 0;
static size_t deallocation_count = 0;

// Override new/delete to track allocations (simple version)
void* operator new(size_t size) {
    allocation_count++;
    return malloc(size);
}

void operator delete(void* ptr) noexcept {
    if (ptr) {
        deallocation_count++;
        free(ptr);
    }
}

void operator delete(void* ptr, size_t) noexcept {
    if (ptr) {
        deallocation_count++;
        free(ptr);
    }
}

void* operator new[](size_t size) {
    allocation_count++;
    return malloc(size);
}

void operator delete[](void* ptr) noexcept {
    if (ptr) {
        deallocation_count++;
        free(ptr);
    }
}

void operator delete[](void* ptr, size_t) noexcept {
    if (ptr) {
        deallocation_count++;
        free(ptr);
    }
}

void test_grid3d_basic_construction_destruction() {
    std::cout << "Test 1: Basic grid3d construction and destruction" << std::endl;

    allocation_count = 0;
    deallocation_count = 0;

    {
        // Create a simple grid
        const float xe[] = {0.0f, 1.0f, 2.0f, 3.0f};
        const float ye[] = {0.0f, 1.0f, 2.0f};
        const float ze[] = {0.0f, 1.0f, 2.0f, 3.0f, 4.0f};

        mqi::grid3d<float, float> grid(xe, 4, ye, 3, ze, 5);

        std::cout << "  Grid created. Allocations: " << allocation_count << std::endl;
        // Should have allocated: xe_ array, ye_ array, ze_ array = 3 allocations

    } // grid goes out of scope here - destructor should be called

    std::cout << "  Grid destroyed. Deallocations: " << deallocation_count << std::endl;

    if (deallocation_count < allocation_count) {
        std::cerr << "  ❌ MEMORY LEAK DETECTED!" << std::endl;
        std::cerr << "     Allocated: " << allocation_count << ", Freed: " << deallocation_count << std::endl;
        std::cerr << "     Leaked: " << (allocation_count - deallocation_count) << " allocations" << std::endl;
    } else {
        std::cout << "  ✓ No memory leaks detected" << std::endl;
    }
    std::cout << std::endl;
}

void test_grid3d_range_construction() {
    std::cout << "Test 2: Grid3d construction with ranges" << std::endl;

    allocation_count = 0;
    deallocation_count = 0;

    {
        // Create grid with min/max ranges
        mqi::grid3d<double, double> grid(0.0, 10.0, 11,  // x: 0-10 with 11 edges
                                          0.0, 5.0, 6,     // y: 0-5 with 6 edges
                                          0.0, 8.0, 9);    // z: 0-8 with 9 edges

        std::cout << "  Grid created. Allocations: " << allocation_count << std::endl;

    } // Destructor called here

    std::cout << "  Grid destroyed. Deallocations: " << deallocation_count << std::endl;

    if (deallocation_count < allocation_count) {
        std::cerr << "  ❌ MEMORY LEAK DETECTED!" << std::endl;
        std::cerr << "     Allocated: " << allocation_count << ", Freed: " << deallocation_count << std::endl;
        std::cerr << "     Leaked: " << (allocation_count - deallocation_count) << " allocations" << std::endl;
    } else {
        std::cout << "  ✓ No memory leaks detected" << std::endl;
    }
    std::cout << std::endl;
}

void test_grid3d_multiple_instances() {
    std::cout << "Test 3: Multiple grid3d instances" << std::endl;

    allocation_count = 0;
    deallocation_count = 0;

    const int num_grids = 10;
    {
        for (int i = 0; i < num_grids; i++) {
            mqi::grid3d<float, float>* grid = new mqi::grid3d<float, float>(
                0.0f, 10.0f, 11,
                0.0f, 10.0f, 11,
                0.0f, 10.0f, 11
            );
            delete grid;  // Should properly clean up
        }
    }

    std::cout << "  Created and destroyed " << num_grids << " grids" << std::endl;
    std::cout << "  Allocations: " << allocation_count << std::endl;
    std::cout << "  Deallocations: " << deallocation_count << std::endl;

    if (deallocation_count < allocation_count) {
        std::cerr << "  ❌ MEMORY LEAK DETECTED!" << std::endl;
        std::cerr << "     Total leaked: " << (allocation_count - deallocation_count) << " allocations" << std::endl;
        std::cerr << "     Per grid: " << (allocation_count - deallocation_count) / num_grids << " allocations" << std::endl;
    } else {
        std::cout << "  ✓ No memory leaks detected" << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Grid3D Memory Leak Test Suite" << std::endl;
    std::cout << "Testing Critical Issue 1.2" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    test_grid3d_basic_construction_destruction();
    test_grid3d_range_construction();
    test_grid3d_multiple_instances();

    std::cout << "========================================" << std::endl;
    std::cout << "All tests completed" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
