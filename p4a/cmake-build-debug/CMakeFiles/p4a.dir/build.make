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
CMAKE_SOURCE_DIR = /tmp/tmp.SIg3Ef97Zg

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /tmp/tmp.SIg3Ef97Zg/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/p4a.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/p4a.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/p4a.dir/flags.make

CMakeFiles/p4a.dir/main.c.o: CMakeFiles/p4a.dir/flags.make
CMakeFiles/p4a.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.SIg3Ef97Zg/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/p4a.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/p4a.dir/main.c.o   -c /tmp/tmp.SIg3Ef97Zg/main.c

CMakeFiles/p4a.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/p4a.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tmp/tmp.SIg3Ef97Zg/main.c > CMakeFiles/p4a.dir/main.c.i

CMakeFiles/p4a.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/p4a.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tmp/tmp.SIg3Ef97Zg/main.c -o CMakeFiles/p4a.dir/main.c.s

CMakeFiles/p4a.dir/main.c.o.requires:

.PHONY : CMakeFiles/p4a.dir/main.c.o.requires

CMakeFiles/p4a.dir/main.c.o.provides: CMakeFiles/p4a.dir/main.c.o.requires
	$(MAKE) -f CMakeFiles/p4a.dir/build.make CMakeFiles/p4a.dir/main.c.o.provides.build
.PHONY : CMakeFiles/p4a.dir/main.c.o.provides

CMakeFiles/p4a.dir/main.c.o.provides.build: CMakeFiles/p4a.dir/main.c.o


# Object files for target p4a
p4a_OBJECTS = \
"CMakeFiles/p4a.dir/main.c.o"

# External object files for target p4a
p4a_EXTERNAL_OBJECTS =

p4a: CMakeFiles/p4a.dir/main.c.o
p4a: CMakeFiles/p4a.dir/build.make
p4a: CMakeFiles/p4a.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/tmp/tmp.SIg3Ef97Zg/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable p4a"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/p4a.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/p4a.dir/build: p4a

.PHONY : CMakeFiles/p4a.dir/build

CMakeFiles/p4a.dir/requires: CMakeFiles/p4a.dir/main.c.o.requires

.PHONY : CMakeFiles/p4a.dir/requires

CMakeFiles/p4a.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/p4a.dir/cmake_clean.cmake
.PHONY : CMakeFiles/p4a.dir/clean

CMakeFiles/p4a.dir/depend:
	cd /tmp/tmp.SIg3Ef97Zg/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /tmp/tmp.SIg3Ef97Zg /tmp/tmp.SIg3Ef97Zg /tmp/tmp.SIg3Ef97Zg/cmake-build-debug /tmp/tmp.SIg3Ef97Zg/cmake-build-debug /tmp/tmp.SIg3Ef97Zg/cmake-build-debug/CMakeFiles/p4a.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/p4a.dir/depend

