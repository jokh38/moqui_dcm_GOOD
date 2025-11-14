/**
 * Test file for treatment machine memory leak fixes
 * Tests Critical Issue 1.3: Memory leaks in treatment machine
 */

#include <iostream>
#include <cassert>

// Simple test for the specific allocations in treatment machine
// We'll test the pattern rather than the full class

// Include headers
#include "../base/mqi_beamlet.hpp"
#include "../base/distributions/mqi_norm_1d.hpp"
#include "../base/distributions/mqi_phsp6d_ray.hpp"

// Memory tracking
static size_t allocation_count = 0;
static size_t deallocation_count = 0;

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

void test_beamlet_creation_pattern() {
    std::cout << "Test 1: Beamlet creation pattern (from treatment machine)" << std::endl;

    allocation_count = 0;
    deallocation_count = 0;

    {
        // Simulate the pattern from characterize_beamlet()
        auto energy = new mqi::norm_1d<double>({150.0}, {0.5});

        std::array<double, 6> beamlet_mean = {0.0, 0.0, -465.0, 0.0, 0.0, -1.0};
        std::array<double, 6> beamlet_sigm = {5.0, 5.0, 0.0, 0.01, 0.01, 0.0};
        std::array<double, 2> beamlet_divergence = {0.001, 0.001};

        auto fluence = new mqi::phsp_6d_ray<double>(beamlet_mean, beamlet_sigm,
                                                     beamlet_divergence, -465.0);

        // This is what happens in the treatment machine code
        mqi::beamlet<double> bl(energy, fluence);

        std::cout << "  Beamlet created. Allocations: " << allocation_count << std::endl;

    } // Beamlet goes out of scope - should destructor delete the pointers?

    std::cout << "  Beamlet destroyed. Deallocations: " << deallocation_count << std::endl;

    if (deallocation_count < allocation_count) {
        std::cerr << "  ❌ MEMORY LEAK DETECTED!" << std::endl;
        std::cerr << "     Allocated: " << allocation_count << ", Freed: " << deallocation_count << std::endl;
        std::cerr << "     Leaked: " << (allocation_count - deallocation_count) << " allocations" << std::endl;
        std::cerr << "     Issue: beamlet destructor doesn't delete energy and fluence pointers" << std::endl;
    } else {
        std::cout << "  ✓ No memory leaks detected" << std::endl;
    }
    std::cout << std::endl;
}

void test_multiple_beamlets() {
    std::cout << "Test 2: Multiple beamlet instances" << std::endl;

    allocation_count = 0;
    deallocation_count = 0;

    const int num_beamlets = 5;
    {
        for (int i = 0; i < num_beamlets; i++) {
            auto energy = new mqi::norm_1d<double>({100.0 + i}, {0.5});

            std::array<double, 6> mean = {0.0, 0.0, -465.0, 0.0, 0.0, -1.0};
            std::array<double, 6> sigm = {5.0, 5.0, 0.0, 0.01, 0.01, 0.0};
            std::array<double, 2> div = {0.001, 0.001};

            auto fluence = new mqi::phsp_6d_ray<double>(mean, sigm, div, -465.0);
            mqi::beamlet<double>* bl = new mqi::beamlet<double>(energy, fluence);

            delete bl;  // Should this cascade to delete energy and fluence?
        }
    }

    std::cout << "  Created and destroyed " << num_beamlets << " beamlets" << std::endl;
    std::cout << "  Allocations: " << allocation_count << std::endl;
    std::cout << "  Deallocations: " << deallocation_count << std::endl;

    if (deallocation_count < allocation_count) {
        std::cerr << "  ❌ MEMORY LEAK DETECTED!" << std::endl;
        std::cerr << "     Total leaked: " << (allocation_count - deallocation_count) << " allocations" << std::endl;
        if (num_beamlets > 0) {
            std::cerr << "     Per beamlet: ~" << (allocation_count - deallocation_count) / num_beamlets << " allocations" << std::endl;
        }
    } else {
        std::cout << "  ✓ No memory leaks detected" << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Treatment Machine Memory Leak Test Suite" << std::endl;
    std::cout << "Testing Critical Issue 1.3" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    test_beamlet_creation_pattern();
    test_multiple_beamlets();

    std::cout << "========================================" << std::endl;
    std::cout << "All tests completed" << std::endl;
    std::cout << "Note: rangeshifter/aperture returns tested separately" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
