# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/bryce/keyrita-revived/build/_deps/debug_logger-src"
  "/home/bryce/keyrita-revived/build/_deps/debug_logger-build"
  "/home/bryce/keyrita-revived/build/_deps/debug_logger-subbuild/debug_logger-populate-prefix"
  "/home/bryce/keyrita-revived/build/_deps/debug_logger-subbuild/debug_logger-populate-prefix/tmp"
  "/home/bryce/keyrita-revived/build/_deps/debug_logger-subbuild/debug_logger-populate-prefix/src/debug_logger-populate-stamp"
  "/home/bryce/keyrita-revived/build/_deps/debug_logger-subbuild/debug_logger-populate-prefix/src"
  "/home/bryce/keyrita-revived/build/_deps/debug_logger-subbuild/debug_logger-populate-prefix/src/debug_logger-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/bryce/keyrita-revived/build/_deps/debug_logger-subbuild/debug_logger-populate-prefix/src/debug_logger-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/bryce/keyrita-revived/build/_deps/debug_logger-subbuild/debug_logger-populate-prefix/src/debug_logger-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
