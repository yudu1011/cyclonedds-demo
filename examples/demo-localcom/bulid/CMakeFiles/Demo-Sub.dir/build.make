# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_SOURCE_DIR = /home/byd/Desktop/cyclonedds/examples/demo-localcom

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/byd/Desktop/cyclonedds/examples/demo-localcom/bulid

# Include any dependencies generated for this target.
include CMakeFiles/Demo-Sub.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Demo-Sub.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Demo-Sub.dir/flags.make

DemoData.c: ../DemoData.idl
DemoData.c: /lib/libcycloneddsidlc.so.0.11.0
DemoData.c: /lib/libcycloneddsidlc.so.0.11.0
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/byd/Desktop/cyclonedds/examples/demo-localcom/bulid/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating DemoData.c, DemoData.h"
	/usr/bin/idlc -l/lib/libcycloneddsidlc.so.0.11.0 -Wno-implicit-extensibility -o/home/byd/Desktop/cyclonedds/examples/demo-localcom/bulid /home/byd/Desktop/cyclonedds/examples/demo-localcom/DemoData.idl

DemoData.h: DemoData.c
	@$(CMAKE_COMMAND) -E touch_nocreate DemoData.h

CMakeFiles/Demo-Sub.dir/sub.c.o: CMakeFiles/Demo-Sub.dir/flags.make
CMakeFiles/Demo-Sub.dir/sub.c.o: ../sub.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/byd/Desktop/cyclonedds/examples/demo-localcom/bulid/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/Demo-Sub.dir/sub.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/Demo-Sub.dir/sub.c.o   -c /home/byd/Desktop/cyclonedds/examples/demo-localcom/sub.c

CMakeFiles/Demo-Sub.dir/sub.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Demo-Sub.dir/sub.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/byd/Desktop/cyclonedds/examples/demo-localcom/sub.c > CMakeFiles/Demo-Sub.dir/sub.c.i

CMakeFiles/Demo-Sub.dir/sub.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Demo-Sub.dir/sub.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/byd/Desktop/cyclonedds/examples/demo-localcom/sub.c -o CMakeFiles/Demo-Sub.dir/sub.c.s

CMakeFiles/Demo-Sub.dir/DemoData.c.o: CMakeFiles/Demo-Sub.dir/flags.make
CMakeFiles/Demo-Sub.dir/DemoData.c.o: DemoData.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/byd/Desktop/cyclonedds/examples/demo-localcom/bulid/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/Demo-Sub.dir/DemoData.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/Demo-Sub.dir/DemoData.c.o   -c /home/byd/Desktop/cyclonedds/examples/demo-localcom/bulid/DemoData.c

CMakeFiles/Demo-Sub.dir/DemoData.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Demo-Sub.dir/DemoData.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/byd/Desktop/cyclonedds/examples/demo-localcom/bulid/DemoData.c > CMakeFiles/Demo-Sub.dir/DemoData.c.i

CMakeFiles/Demo-Sub.dir/DemoData.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Demo-Sub.dir/DemoData.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/byd/Desktop/cyclonedds/examples/demo-localcom/bulid/DemoData.c -o CMakeFiles/Demo-Sub.dir/DemoData.c.s

# Object files for target Demo-Sub
Demo__Sub_OBJECTS = \
"CMakeFiles/Demo-Sub.dir/sub.c.o" \
"CMakeFiles/Demo-Sub.dir/DemoData.c.o"

# External object files for target Demo-Sub
Demo__Sub_EXTERNAL_OBJECTS =

Demo-Sub: CMakeFiles/Demo-Sub.dir/sub.c.o
Demo-Sub: CMakeFiles/Demo-Sub.dir/DemoData.c.o
Demo-Sub: CMakeFiles/Demo-Sub.dir/build.make
Demo-Sub: /lib/libddsc.so.0.11.0
Demo-Sub: CMakeFiles/Demo-Sub.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/byd/Desktop/cyclonedds/examples/demo-localcom/bulid/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking C executable Demo-Sub"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Demo-Sub.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Demo-Sub.dir/build: Demo-Sub

.PHONY : CMakeFiles/Demo-Sub.dir/build

CMakeFiles/Demo-Sub.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Demo-Sub.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Demo-Sub.dir/clean

CMakeFiles/Demo-Sub.dir/depend: DemoData.c
CMakeFiles/Demo-Sub.dir/depend: DemoData.h
	cd /home/byd/Desktop/cyclonedds/examples/demo-localcom/bulid && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/byd/Desktop/cyclonedds/examples/demo-localcom /home/byd/Desktop/cyclonedds/examples/demo-localcom /home/byd/Desktop/cyclonedds/examples/demo-localcom/bulid /home/byd/Desktop/cyclonedds/examples/demo-localcom/bulid /home/byd/Desktop/cyclonedds/examples/demo-localcom/bulid/CMakeFiles/Demo-Sub.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Demo-Sub.dir/depend
