# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/flast/Documents/dev/flast

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/flast/Documents/dev/flast/build

# Include any dependencies generated for this target.
include CMakeFiles/flast.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/flast.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/flast.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/flast.dir/flags.make

CMakeFiles/flast.dir/src/CodeGen.cpp.o: CMakeFiles/flast.dir/flags.make
CMakeFiles/flast.dir/src/CodeGen.cpp.o: ../src/CodeGen.cpp
CMakeFiles/flast.dir/src/CodeGen.cpp.o: CMakeFiles/flast.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/flast/Documents/dev/flast/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/flast.dir/src/CodeGen.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/flast.dir/src/CodeGen.cpp.o -MF CMakeFiles/flast.dir/src/CodeGen.cpp.o.d -o CMakeFiles/flast.dir/src/CodeGen.cpp.o -c /home/flast/Documents/dev/flast/src/CodeGen.cpp

CMakeFiles/flast.dir/src/CodeGen.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/flast.dir/src/CodeGen.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/flast/Documents/dev/flast/src/CodeGen.cpp > CMakeFiles/flast.dir/src/CodeGen.cpp.i

CMakeFiles/flast.dir/src/CodeGen.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/flast.dir/src/CodeGen.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/flast/Documents/dev/flast/src/CodeGen.cpp -o CMakeFiles/flast.dir/src/CodeGen.cpp.s

CMakeFiles/flast.dir/src/ErrorHandler.cpp.o: CMakeFiles/flast.dir/flags.make
CMakeFiles/flast.dir/src/ErrorHandler.cpp.o: ../src/ErrorHandler.cpp
CMakeFiles/flast.dir/src/ErrorHandler.cpp.o: CMakeFiles/flast.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/flast/Documents/dev/flast/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/flast.dir/src/ErrorHandler.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/flast.dir/src/ErrorHandler.cpp.o -MF CMakeFiles/flast.dir/src/ErrorHandler.cpp.o.d -o CMakeFiles/flast.dir/src/ErrorHandler.cpp.o -c /home/flast/Documents/dev/flast/src/ErrorHandler.cpp

CMakeFiles/flast.dir/src/ErrorHandler.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/flast.dir/src/ErrorHandler.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/flast/Documents/dev/flast/src/ErrorHandler.cpp > CMakeFiles/flast.dir/src/ErrorHandler.cpp.i

CMakeFiles/flast.dir/src/ErrorHandler.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/flast.dir/src/ErrorHandler.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/flast/Documents/dev/flast/src/ErrorHandler.cpp -o CMakeFiles/flast.dir/src/ErrorHandler.cpp.s

CMakeFiles/flast.dir/src/Lexer.cpp.o: CMakeFiles/flast.dir/flags.make
CMakeFiles/flast.dir/src/Lexer.cpp.o: ../src/Lexer.cpp
CMakeFiles/flast.dir/src/Lexer.cpp.o: CMakeFiles/flast.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/flast/Documents/dev/flast/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/flast.dir/src/Lexer.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/flast.dir/src/Lexer.cpp.o -MF CMakeFiles/flast.dir/src/Lexer.cpp.o.d -o CMakeFiles/flast.dir/src/Lexer.cpp.o -c /home/flast/Documents/dev/flast/src/Lexer.cpp

CMakeFiles/flast.dir/src/Lexer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/flast.dir/src/Lexer.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/flast/Documents/dev/flast/src/Lexer.cpp > CMakeFiles/flast.dir/src/Lexer.cpp.i

CMakeFiles/flast.dir/src/Lexer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/flast.dir/src/Lexer.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/flast/Documents/dev/flast/src/Lexer.cpp -o CMakeFiles/flast.dir/src/Lexer.cpp.s

CMakeFiles/flast.dir/src/Parser.cpp.o: CMakeFiles/flast.dir/flags.make
CMakeFiles/flast.dir/src/Parser.cpp.o: ../src/Parser.cpp
CMakeFiles/flast.dir/src/Parser.cpp.o: CMakeFiles/flast.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/flast/Documents/dev/flast/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/flast.dir/src/Parser.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/flast.dir/src/Parser.cpp.o -MF CMakeFiles/flast.dir/src/Parser.cpp.o.d -o CMakeFiles/flast.dir/src/Parser.cpp.o -c /home/flast/Documents/dev/flast/src/Parser.cpp

CMakeFiles/flast.dir/src/Parser.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/flast.dir/src/Parser.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/flast/Documents/dev/flast/src/Parser.cpp > CMakeFiles/flast.dir/src/Parser.cpp.i

CMakeFiles/flast.dir/src/Parser.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/flast.dir/src/Parser.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/flast/Documents/dev/flast/src/Parser.cpp -o CMakeFiles/flast.dir/src/Parser.cpp.s

CMakeFiles/flast.dir/src/SemanticAnalyzer.cpp.o: CMakeFiles/flast.dir/flags.make
CMakeFiles/flast.dir/src/SemanticAnalyzer.cpp.o: ../src/SemanticAnalyzer.cpp
CMakeFiles/flast.dir/src/SemanticAnalyzer.cpp.o: CMakeFiles/flast.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/flast/Documents/dev/flast/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/flast.dir/src/SemanticAnalyzer.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/flast.dir/src/SemanticAnalyzer.cpp.o -MF CMakeFiles/flast.dir/src/SemanticAnalyzer.cpp.o.d -o CMakeFiles/flast.dir/src/SemanticAnalyzer.cpp.o -c /home/flast/Documents/dev/flast/src/SemanticAnalyzer.cpp

CMakeFiles/flast.dir/src/SemanticAnalyzer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/flast.dir/src/SemanticAnalyzer.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/flast/Documents/dev/flast/src/SemanticAnalyzer.cpp > CMakeFiles/flast.dir/src/SemanticAnalyzer.cpp.i

CMakeFiles/flast.dir/src/SemanticAnalyzer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/flast.dir/src/SemanticAnalyzer.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/flast/Documents/dev/flast/src/SemanticAnalyzer.cpp -o CMakeFiles/flast.dir/src/SemanticAnalyzer.cpp.s

CMakeFiles/flast.dir/src/main.cpp.o: CMakeFiles/flast.dir/flags.make
CMakeFiles/flast.dir/src/main.cpp.o: ../src/main.cpp
CMakeFiles/flast.dir/src/main.cpp.o: CMakeFiles/flast.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/flast/Documents/dev/flast/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/flast.dir/src/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/flast.dir/src/main.cpp.o -MF CMakeFiles/flast.dir/src/main.cpp.o.d -o CMakeFiles/flast.dir/src/main.cpp.o -c /home/flast/Documents/dev/flast/src/main.cpp

CMakeFiles/flast.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/flast.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/flast/Documents/dev/flast/src/main.cpp > CMakeFiles/flast.dir/src/main.cpp.i

CMakeFiles/flast.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/flast.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/flast/Documents/dev/flast/src/main.cpp -o CMakeFiles/flast.dir/src/main.cpp.s

# Object files for target flast
flast_OBJECTS = \
"CMakeFiles/flast.dir/src/CodeGen.cpp.o" \
"CMakeFiles/flast.dir/src/ErrorHandler.cpp.o" \
"CMakeFiles/flast.dir/src/Lexer.cpp.o" \
"CMakeFiles/flast.dir/src/Parser.cpp.o" \
"CMakeFiles/flast.dir/src/SemanticAnalyzer.cpp.o" \
"CMakeFiles/flast.dir/src/main.cpp.o"

# External object files for target flast
flast_EXTERNAL_OBJECTS =

flast: CMakeFiles/flast.dir/src/CodeGen.cpp.o
flast: CMakeFiles/flast.dir/src/ErrorHandler.cpp.o
flast: CMakeFiles/flast.dir/src/Lexer.cpp.o
flast: CMakeFiles/flast.dir/src/Parser.cpp.o
flast: CMakeFiles/flast.dir/src/SemanticAnalyzer.cpp.o
flast: CMakeFiles/flast.dir/src/main.cpp.o
flast: CMakeFiles/flast.dir/build.make
flast: /usr/lib/llvm-14/lib/libLLVMSupport.a
flast: /usr/lib/llvm-14/lib/libLLVMCore.a
flast: /usr/lib/llvm-14/lib/libLLVMIRReader.a
flast: /usr/lib/llvm-14/lib/libLLVMAnalysis.a
flast: /usr/lib/llvm-14/lib/libLLVMExecutionEngine.a
flast: /usr/lib/llvm-14/lib/libLLVMInstCombine.a
flast: /usr/lib/llvm-14/lib/libLLVMObject.a
flast: /usr/lib/llvm-14/lib/libLLVMRuntimeDyld.a
flast: /usr/lib/llvm-14/lib/libLLVMScalarOpts.a
flast: /usr/lib/llvm-14/lib/libLLVMTransformUtils.a
flast: /usr/lib/llvm-14/lib/libLLVMCodeGen.a
flast: /usr/lib/llvm-14/lib/libLLVMTarget.a
flast: /usr/lib/llvm-14/lib/libLLVMAsmParser.a
flast: /usr/lib/llvm-14/lib/libLLVMAsmPrinter.a
flast: /usr/lib/llvm-14/lib/libLLVMX86CodeGen.a
flast: /usr/lib/llvm-14/lib/libLLVMX86AsmParser.a
flast: /usr/lib/llvm-14/lib/libLLVMX86Desc.a
flast: /usr/lib/llvm-14/lib/libLLVMX86Disassembler.a
flast: /usr/lib/llvm-14/lib/libLLVMX86Info.a
flast: /usr/lib/llvm-14/lib/libLLVMOrcTargetProcess.a
flast: /usr/lib/llvm-14/lib/libLLVMOrcShared.a
flast: /usr/lib/llvm-14/lib/libLLVMAsmPrinter.a
flast: /usr/lib/llvm-14/lib/libLLVMDebugInfoMSF.a
flast: /usr/lib/llvm-14/lib/libLLVMInstrumentation.a
flast: /usr/lib/llvm-14/lib/libLLVMGlobalISel.a
flast: /usr/lib/llvm-14/lib/libLLVMSelectionDAG.a
flast: /usr/lib/llvm-14/lib/libLLVMCodeGen.a
flast: /usr/lib/llvm-14/lib/libLLVMScalarOpts.a
flast: /usr/lib/llvm-14/lib/libLLVMInstCombine.a
flast: /usr/lib/llvm-14/lib/libLLVMAggressiveInstCombine.a
flast: /usr/lib/llvm-14/lib/libLLVMTransformUtils.a
flast: /usr/lib/llvm-14/lib/libLLVMBitWriter.a
flast: /usr/lib/llvm-14/lib/libLLVMTarget.a
flast: /usr/lib/llvm-14/lib/libLLVMAnalysis.a
flast: /usr/lib/llvm-14/lib/libLLVMProfileData.a
flast: /usr/lib/llvm-14/lib/libLLVMDebugInfoDWARF.a
flast: /usr/lib/llvm-14/lib/libLLVMObject.a
flast: /usr/lib/llvm-14/lib/libLLVMBitReader.a
flast: /usr/lib/llvm-14/lib/libLLVMTextAPI.a
flast: /usr/lib/llvm-14/lib/libLLVMCFGuard.a
flast: /usr/lib/llvm-14/lib/libLLVMCore.a
flast: /usr/lib/llvm-14/lib/libLLVMRemarks.a
flast: /usr/lib/llvm-14/lib/libLLVMBitstreamReader.a
flast: /usr/lib/llvm-14/lib/libLLVMMCParser.a
flast: /usr/lib/llvm-14/lib/libLLVMMCDisassembler.a
flast: /usr/lib/llvm-14/lib/libLLVMMC.a
flast: /usr/lib/llvm-14/lib/libLLVMBinaryFormat.a
flast: /usr/lib/llvm-14/lib/libLLVMDebugInfoCodeView.a
flast: /usr/lib/llvm-14/lib/libLLVMSupport.a
flast: /usr/lib/x86_64-linux-gnu/libz.so
flast: /usr/lib/x86_64-linux-gnu/libtinfo.so
flast: /usr/lib/llvm-14/lib/libLLVMDemangle.a
flast: CMakeFiles/flast.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/flast/Documents/dev/flast/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Linking CXX executable flast"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/flast.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/flast.dir/build: flast
.PHONY : CMakeFiles/flast.dir/build

CMakeFiles/flast.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/flast.dir/cmake_clean.cmake
.PHONY : CMakeFiles/flast.dir/clean

CMakeFiles/flast.dir/depend:
	cd /home/flast/Documents/dev/flast/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/flast/Documents/dev/flast /home/flast/Documents/dev/flast /home/flast/Documents/dev/flast/build /home/flast/Documents/dev/flast/build /home/flast/Documents/dev/flast/build/CMakeFiles/flast.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/flast.dir/depend

