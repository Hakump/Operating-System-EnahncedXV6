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
CMAKE_SOURCE_DIR = /afs/cs.wisc.edu/u/z/h/zhikang/private/CS537/p5

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /afs/cs.wisc.edu/u/z/h/zhikang/private/CS537/p5/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/p5.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/p5.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/p5.dir/flags.make

CMakeFiles/p5.dir/xcheck.c.o: CMakeFiles/p5.dir/flags.make
CMakeFiles/p5.dir/xcheck.c.o: ../xcheck.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/afs/cs.wisc.edu/u/z/h/zhikang/private/CS537/p5/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/p5.dir/xcheck.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/p5.dir/xcheck.c.o   -c /afs/cs.wisc.edu/u/z/h/zhikang/private/CS537/p5/xcheck.c

CMakeFiles/p5.dir/xcheck.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/p5.dir/xcheck.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /afs/cs.wisc.edu/u/z/h/zhikang/private/CS537/p5/xcheck.c > CMakeFiles/p5.dir/xcheck.c.i

CMakeFiles/p5.dir/xcheck.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/p5.dir/xcheck.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /afs/cs.wisc.edu/u/z/h/zhikang/private/CS537/p5/xcheck.c -o CMakeFiles/p5.dir/xcheck.c.s

CMakeFiles/p5.dir/xcheck.c.o.requires:

.PHONY : CMakeFiles/p5.dir/xcheck.c.o.requires

CMakeFiles/p5.dir/xcheck.c.o.provides: CMakeFiles/p5.dir/xcheck.c.o.requires
	$(MAKE) -f CMakeFiles/p5.dir/build.make CMakeFiles/p5.dir/xcheck.c.o.provides.build
.PHONY : CMakeFiles/p5.dir/xcheck.c.o.provides

CMakeFiles/p5.dir/xcheck.c.o.provides.build: CMakeFiles/p5.dir/xcheck.c.o


# Object files for target p5
p5_OBJECTS = \
"CMakeFiles/p5.dir/xcheck.c.o"

# External object files for target p5
p5_EXTERNAL_OBJECTS =

p5: CMakeFiles/p5.dir/xcheck.c.o
p5: CMakeFiles/p5.dir/build.make
p5: CMakeFiles/p5.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/afs/cs.wisc.edu/u/z/h/zhikang/private/CS537/p5/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable p5"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/p5.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/p5.dir/build: p5

.PHONY : CMakeFiles/p5.dir/build

CMakeFiles/p5.dir/requires: CMakeFiles/p5.dir/xcheck.c.o.requires

.PHONY : CMakeFiles/p5.dir/requires

CMakeFiles/p5.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/p5.dir/cmake_clean.cmake
.PHONY : CMakeFiles/p5.dir/clean

CMakeFiles/p5.dir/depend:
	cd /afs/cs.wisc.edu/u/z/h/zhikang/private/CS537/p5/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /afs/cs.wisc.edu/u/z/h/zhikang/private/CS537/p5 /afs/cs.wisc.edu/u/z/h/zhikang/private/CS537/p5 /afs/cs.wisc.edu/u/z/h/zhikang/private/CS537/p5/cmake-build-debug /afs/cs.wisc.edu/u/z/h/zhikang/private/CS537/p5/cmake-build-debug /afs/cs.wisc.edu/u/z/h/zhikang/private/CS537/p5/cmake-build-debug/CMakeFiles/p5.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/p5.dir/depend
