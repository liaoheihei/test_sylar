# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Produce verbose output by default.
VERBOSE = 1

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/fred/workspace/sylar

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/fred/workspace/sylar/build

# Include any dependencies generated for this target.
include CMakeFiles/test_thread.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test_thread.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_thread.dir/flags.make

CMakeFiles/test_thread.dir/tests/test_thread.cc.o: CMakeFiles/test_thread.dir/flags.make
CMakeFiles/test_thread.dir/tests/test_thread.cc.o: ../tests/test_thread.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fred/workspace/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/test_thread.dir/tests/test_thread.cc.o"
	/usr/bin/x86_64-linux-gnu-g++-7  $(CXX_DEFINES) -D__FILE__=\"tests/test_thread.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_thread.dir/tests/test_thread.cc.o -c /home/fred/workspace/sylar/tests/test_thread.cc

CMakeFiles/test_thread.dir/tests/test_thread.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_thread.dir/tests/test_thread.cc.i"
	/usr/bin/x86_64-linux-gnu-g++-7 $(CXX_DEFINES) -D__FILE__=\"tests/test_thread.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fred/workspace/sylar/tests/test_thread.cc > CMakeFiles/test_thread.dir/tests/test_thread.cc.i

CMakeFiles/test_thread.dir/tests/test_thread.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_thread.dir/tests/test_thread.cc.s"
	/usr/bin/x86_64-linux-gnu-g++-7 $(CXX_DEFINES) -D__FILE__=\"tests/test_thread.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fred/workspace/sylar/tests/test_thread.cc -o CMakeFiles/test_thread.dir/tests/test_thread.cc.s

CMakeFiles/test_thread.dir/tests/test_thread.cc.o.requires:

.PHONY : CMakeFiles/test_thread.dir/tests/test_thread.cc.o.requires

CMakeFiles/test_thread.dir/tests/test_thread.cc.o.provides: CMakeFiles/test_thread.dir/tests/test_thread.cc.o.requires
	$(MAKE) -f CMakeFiles/test_thread.dir/build.make CMakeFiles/test_thread.dir/tests/test_thread.cc.o.provides.build
.PHONY : CMakeFiles/test_thread.dir/tests/test_thread.cc.o.provides

CMakeFiles/test_thread.dir/tests/test_thread.cc.o.provides.build: CMakeFiles/test_thread.dir/tests/test_thread.cc.o


# Object files for target test_thread
test_thread_OBJECTS = \
"CMakeFiles/test_thread.dir/tests/test_thread.cc.o"

# External object files for target test_thread
test_thread_EXTERNAL_OBJECTS =

../bin/test_thread: CMakeFiles/test_thread.dir/tests/test_thread.cc.o
../bin/test_thread: CMakeFiles/test_thread.dir/build.make
../bin/test_thread: ../lib/libsylar.so
../bin/test_thread: /usr/local/lib/libyaml-cpp.a
../bin/test_thread: CMakeFiles/test_thread.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/fred/workspace/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/test_thread"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_thread.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_thread.dir/build: ../bin/test_thread

.PHONY : CMakeFiles/test_thread.dir/build

CMakeFiles/test_thread.dir/requires: CMakeFiles/test_thread.dir/tests/test_thread.cc.o.requires

.PHONY : CMakeFiles/test_thread.dir/requires

CMakeFiles/test_thread.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_thread.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_thread.dir/clean

CMakeFiles/test_thread.dir/depend:
	cd /home/fred/workspace/sylar/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/fred/workspace/sylar /home/fred/workspace/sylar /home/fred/workspace/sylar/build /home/fred/workspace/sylar/build /home/fred/workspace/sylar/build/CMakeFiles/test_thread.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_thread.dir/depend

