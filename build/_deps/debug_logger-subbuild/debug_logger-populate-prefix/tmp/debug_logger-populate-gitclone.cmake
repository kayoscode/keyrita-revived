# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

if(EXISTS "/home/bryce/keyrita-revived/build/_deps/debug_logger-subbuild/debug_logger-populate-prefix/src/debug_logger-populate-stamp/debug_logger-populate-gitclone-lastrun.txt" AND EXISTS "/home/bryce/keyrita-revived/build/_deps/debug_logger-subbuild/debug_logger-populate-prefix/src/debug_logger-populate-stamp/debug_logger-populate-gitinfo.txt" AND
  "/home/bryce/keyrita-revived/build/_deps/debug_logger-subbuild/debug_logger-populate-prefix/src/debug_logger-populate-stamp/debug_logger-populate-gitclone-lastrun.txt" IS_NEWER_THAN "/home/bryce/keyrita-revived/build/_deps/debug_logger-subbuild/debug_logger-populate-prefix/src/debug_logger-populate-stamp/debug_logger-populate-gitinfo.txt")
  message(STATUS
    "Avoiding repeated git clone, stamp file is up to date: "
    "'/home/bryce/keyrita-revived/build/_deps/debug_logger-subbuild/debug_logger-populate-prefix/src/debug_logger-populate-stamp/debug_logger-populate-gitclone-lastrun.txt'"
  )
  return()
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E rm -rf "/home/bryce/keyrita-revived/build/_deps/debug_logger-src"
  RESULT_VARIABLE error_code
)
if(error_code)
  message(FATAL_ERROR "Failed to remove directory: '/home/bryce/keyrita-revived/build/_deps/debug_logger-src'")
endif()

# try the clone 3 times in case there is an odd git clone issue
set(error_code 1)
set(number_of_tries 0)
while(error_code AND number_of_tries LESS 3)
  execute_process(
    COMMAND "/usr/bin/git"
            clone --no-checkout --config "advice.detachedHead=false" "git@github.com:youngbrycecode/DebugLogger.git" "debug_logger-src"
    WORKING_DIRECTORY "/home/bryce/keyrita-revived/build/_deps"
    RESULT_VARIABLE error_code
  )
  math(EXPR number_of_tries "${number_of_tries} + 1")
endwhile()
if(number_of_tries GREATER 1)
  message(STATUS "Had to git clone more than once: ${number_of_tries} times.")
endif()
if(error_code)
  message(FATAL_ERROR "Failed to clone repository: 'git@github.com:youngbrycecode/DebugLogger.git'")
endif()

execute_process(
  COMMAND "/usr/bin/git"
          checkout "release1.0" --
  WORKING_DIRECTORY "/home/bryce/keyrita-revived/build/_deps/debug_logger-src"
  RESULT_VARIABLE error_code
)
if(error_code)
  message(FATAL_ERROR "Failed to checkout tag: 'release1.0'")
endif()

set(init_submodules TRUE)
if(init_submodules)
  execute_process(
    COMMAND "/usr/bin/git" 
            submodule update --recursive --init 
    WORKING_DIRECTORY "/home/bryce/keyrita-revived/build/_deps/debug_logger-src"
    RESULT_VARIABLE error_code
  )
endif()
if(error_code)
  message(FATAL_ERROR "Failed to update submodules in: '/home/bryce/keyrita-revived/build/_deps/debug_logger-src'")
endif()

# Complete success, update the script-last-run stamp file:
#
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy "/home/bryce/keyrita-revived/build/_deps/debug_logger-subbuild/debug_logger-populate-prefix/src/debug_logger-populate-stamp/debug_logger-populate-gitinfo.txt" "/home/bryce/keyrita-revived/build/_deps/debug_logger-subbuild/debug_logger-populate-prefix/src/debug_logger-populate-stamp/debug_logger-populate-gitclone-lastrun.txt"
  RESULT_VARIABLE error_code
)
if(error_code)
  message(FATAL_ERROR "Failed to copy script-last-run stamp file: '/home/bryce/keyrita-revived/build/_deps/debug_logger-subbuild/debug_logger-populate-prefix/src/debug_logger-populate-stamp/debug_logger-populate-gitclone-lastrun.txt'")
endif()
