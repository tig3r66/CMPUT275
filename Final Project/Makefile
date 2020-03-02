# ==================================
# Name: Edward (Eddie) Guo
# ID: 1576381
# CMPUT 275, Winter 2020
#
# Final Project: <INSERT NAME HERE>
# ==================================

# ===========
# Description
# ===========
# Builds all .cpp files listed after SRC.

# =====
# Notes
# =====
	# Includes the necessary files and libraries to use the matplotlib-cpp
	# Python wrapper.

	# Requires that Python 2, Python >=3.6, matplotlib, and numpy to be
	# installed. Avoid including numpy by using the flag -DWITHOUT_NUMPY.

	# This Makefile takes inspiration from Benno Evers' GitHub repository
	# lava/matplotlib-cpp licensed under The MIT License.
	# Link: https://github.com/lava/matplotlib-cpp
	# Documentation: https://matplotlib-cpp.readthedocs.io/en/latest/

# Use C++11
CXXFLAGS += -std=c++11

# Default to using system's default version of python
PYTHON_BIN     ?= python
PYTHON_CONFIG  := $(PYTHON_BIN)-config
PYTHON_INCLUDE ?= $(shell $(PYTHON_CONFIG) --includes)
CXXFLAGS       += $(PYTHON_INCLUDE)
LDFLAGS        += $(shell $(PYTHON_CONFIG) --libs)

# Either finds numpy or set -DWITHOUT_NUMPY
CXXFLAGS        += $(shell $(PYTHON_BIN) $(CURDIR)/include/numpy_flags.py)
WITHOUT_NUMPY   := $(findstring $(CXXFLAGS), WITHOUT_NUMPY)

# Compile these executables
SRC := main
# Examples requiring numpy support to compile
# SRC_NUMPY  := surface
# SRC        := minimal basic modern animation nonblock xkcd quiver bar fill_inbetween fill update subplot2grid \
#                    $(if WITHOUT_NUMPY,,$(SRC_NUMPY))

# Prefix every example with 'main/build/'
TARGETS := $(patsubst %,build/%,$(SRC))

.PHONY: src

src: $(TARGETS)

# Assume every *.cpp file is a separate example
$(TARGETS): build/%: src/%.cpp
	mkdir -p build
	$(CXX) -o $@ $< $(CXXFLAGS) $(LDFLAGS) -DWITHOUT_NUMPY

clean:
	@rm -f $(TARGETS)
	@rmdir build
	@echo All executables successfully removed.
