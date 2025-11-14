# Simple Makefile for MOQUI tests
CXX = g++
CXXFLAGS = -std=c++17 -I.. -Wall -Wextra
LDFLAGS =

# Test executables
TEST_DICOM_HEADER = test_dicom_header
TEST_IO_COMMON = test_io_common

all: $(TEST_DICOM_HEADER) $(TEST_IO_COMMON)

$(TEST_DICOM_HEADER): test_dicom_header.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

$(TEST_IO_COMMON): test_io_common.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

run_tests: all
	@echo "==================================="
	@echo "Running DICOM header tests..."
	@echo "==================================="
	./$(TEST_DICOM_HEADER)
	@echo ""
	@echo "==================================="
	@echo "Running IO common tests..."
	@echo "==================================="
	./$(TEST_IO_COMMON)

clean:
	rm -f $(TEST_DICOM_HEADER) $(TEST_IO_COMMON)

.PHONY: all run_tests clean
