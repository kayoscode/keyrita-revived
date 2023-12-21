# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/Bryce/Desktop/keyrita-revived/vsproj/_deps/debug_logger-src"
  "C:/Users/Bryce/Desktop/keyrita-revived/vsproj/_deps/debug_logger-build"
  "C:/Users/Bryce/Desktop/keyrita-revived/vsproj/_deps/debug_logger-subbuild/debug_logger-populate-prefix"
  "C:/Users/Bryce/Desktop/keyrita-revived/vsproj/_deps/debug_logger-subbuild/debug_logger-populate-prefix/tmp"
  "C:/Users/Bryce/Desktop/keyrita-revived/vsproj/_deps/debug_logger-subbuild/debug_logger-populate-prefix/src/debug_logger-populate-stamp"
  "C:/Users/Bryce/Desktop/keyrita-revived/vsproj/_deps/debug_logger-subbuild/debug_logger-populate-prefix/src"
  "C:/Users/Bryce/Desktop/keyrita-revived/vsproj/_deps/debug_logger-subbuild/debug_logger-populate-prefix/src/debug_logger-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/Bryce/Desktop/keyrita-revived/vsproj/_deps/debug_logger-subbuild/debug_logger-populate-prefix/src/debug_logger-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/Bryce/Desktop/keyrita-revived/vsproj/_deps/debug_logger-subbuild/debug_logger-populate-prefix/src/debug_logger-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
