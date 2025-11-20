include(cmake/SystemLink.cmake)
include(cmake/LibFuzzer.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)


include(CheckCXXSourceCompiles)


macro(decision_tree_supports_sanitizers)
  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND NOT WIN32)

    message(STATUS "Sanity checking UndefinedBehaviorSanitizer, it should be supported on this platform")
    set(TEST_PROGRAM "int main() { return 0; }")

    # Check if UndefinedBehaviorSanitizer works at link time
    set(CMAKE_REQUIRED_FLAGS "-fsanitize=undefined")
    set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=undefined")
    check_cxx_source_compiles("${TEST_PROGRAM}" HAS_UBSAN_LINK_SUPPORT)

    if(HAS_UBSAN_LINK_SUPPORT)
      message(STATUS "UndefinedBehaviorSanitizer is supported at both compile and link time.")
      set(SUPPORTS_UBSAN ON)
    else()
      message(WARNING "UndefinedBehaviorSanitizer is NOT supported at link time.")
      set(SUPPORTS_UBSAN OFF)
    endif()
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND WIN32)
    set(SUPPORTS_ASAN OFF)
  else()
    if (NOT WIN32)
      message(STATUS "Sanity checking AddressSanitizer, it should be supported on this platform")
      set(TEST_PROGRAM "int main() { return 0; }")

      # Check if AddressSanitizer works at link time
      set(CMAKE_REQUIRED_FLAGS "-fsanitize=address")
      set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=address")
      check_cxx_source_compiles("${TEST_PROGRAM}" HAS_ASAN_LINK_SUPPORT)

      if(HAS_ASAN_LINK_SUPPORT)
        message(STATUS "AddressSanitizer is supported at both compile and link time.")
        set(SUPPORTS_ASAN ON)
      else()
        message(WARNING "AddressSanitizer is NOT supported at link time.")
        set(SUPPORTS_ASAN OFF)
      endif()
    else()
      set(SUPPORTS_ASAN ON)
    endif()
  endif()
endmacro()

macro(decision_tree_setup_options)
  option(decision_tree_ENABLE_HARDENING "Enable hardening" ON)
  option(decision_tree_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    decision_tree_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
    ON
    decision_tree_ENABLE_HARDENING
    OFF)

  decision_tree_supports_sanitizers()

  if(NOT PROJECT_IS_TOP_LEVEL OR decision_tree_PACKAGING_MAINTAINER_MODE)
    option(decision_tree_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(decision_tree_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(decision_tree_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(decision_tree_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(decision_tree_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(decision_tree_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(decision_tree_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(decision_tree_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(decision_tree_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(decision_tree_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(decision_tree_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(decision_tree_ENABLE_PCH "Enable precompiled headers" OFF)
    option(decision_tree_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(decision_tree_ENABLE_IPO "Enable IPO/LTO" ON)
    option(decision_tree_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(decision_tree_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(decision_tree_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" ${SUPPORTS_ASAN})
    option(decision_tree_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(decision_tree_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" ${SUPPORTS_UBSAN})
    option(decision_tree_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(decision_tree_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(decision_tree_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(decision_tree_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(decision_tree_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(decision_tree_ENABLE_PCH "Enable precompiled headers" OFF)
    option(decision_tree_ENABLE_CACHE "Enable ccache" ON)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      decision_tree_ENABLE_IPO
      decision_tree_WARNINGS_AS_ERRORS
      decision_tree_ENABLE_USER_LINKER
      decision_tree_ENABLE_SANITIZER_ADDRESS
      decision_tree_ENABLE_SANITIZER_LEAK
      decision_tree_ENABLE_SANITIZER_UNDEFINED
      decision_tree_ENABLE_SANITIZER_THREAD
      decision_tree_ENABLE_SANITIZER_MEMORY
      decision_tree_ENABLE_UNITY_BUILD
      decision_tree_ENABLE_CLANG_TIDY
      decision_tree_ENABLE_CPPCHECK
      decision_tree_ENABLE_COVERAGE
      decision_tree_ENABLE_PCH
      decision_tree_ENABLE_CACHE)
  endif()

  decision_tree_check_libfuzzer_support(LIBFUZZER_SUPPORTED)
  if(LIBFUZZER_SUPPORTED AND (decision_tree_ENABLE_SANITIZER_ADDRESS OR decision_tree_ENABLE_SANITIZER_THREAD OR decision_tree_ENABLE_SANITIZER_UNDEFINED))
    set(DEFAULT_FUZZER ON)
  else()
    set(DEFAULT_FUZZER OFF)
  endif()

  option(decision_tree_BUILD_FUZZ_TESTS "Enable fuzz testing executable" ${DEFAULT_FUZZER})

endmacro()

macro(decision_tree_global_options)
  if(decision_tree_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    decision_tree_enable_ipo()
  endif()

  decision_tree_supports_sanitizers()

  if(decision_tree_ENABLE_HARDENING AND decision_tree_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR decision_tree_ENABLE_SANITIZER_UNDEFINED
       OR decision_tree_ENABLE_SANITIZER_ADDRESS
       OR decision_tree_ENABLE_SANITIZER_THREAD
       OR decision_tree_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    message("${decision_tree_ENABLE_HARDENING} ${ENABLE_UBSAN_MINIMAL_RUNTIME} ${decision_tree_ENABLE_SANITIZER_UNDEFINED}")
    decision_tree_enable_hardening(decision_tree_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(decision_tree_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(decision_tree_warnings INTERFACE)
  add_library(decision_tree_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  decision_tree_set_project_warnings(
    decision_tree_warnings
    ${decision_tree_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  if(decision_tree_ENABLE_USER_LINKER)
    include(cmake/Linker.cmake)
    decision_tree_configure_linker(decision_tree_options)
  endif()

  include(cmake/Sanitizers.cmake)
  decision_tree_enable_sanitizers(
    decision_tree_options
    ${decision_tree_ENABLE_SANITIZER_ADDRESS}
    ${decision_tree_ENABLE_SANITIZER_LEAK}
    ${decision_tree_ENABLE_SANITIZER_UNDEFINED}
    ${decision_tree_ENABLE_SANITIZER_THREAD}
    ${decision_tree_ENABLE_SANITIZER_MEMORY})

  set_target_properties(decision_tree_options PROPERTIES UNITY_BUILD ${decision_tree_ENABLE_UNITY_BUILD})

  if(decision_tree_ENABLE_PCH)
    target_precompile_headers(
      decision_tree_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(decision_tree_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    decision_tree_enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(decision_tree_ENABLE_CLANG_TIDY)
    decision_tree_enable_clang_tidy(decision_tree_options ${decision_tree_WARNINGS_AS_ERRORS})
  endif()

  if(decision_tree_ENABLE_CPPCHECK)
    decision_tree_enable_cppcheck(${decision_tree_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(decision_tree_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    decision_tree_enable_coverage(decision_tree_options)
  endif()

  if(decision_tree_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(decision_tree_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(decision_tree_ENABLE_HARDENING AND NOT decision_tree_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR decision_tree_ENABLE_SANITIZER_UNDEFINED
       OR decision_tree_ENABLE_SANITIZER_ADDRESS
       OR decision_tree_ENABLE_SANITIZER_THREAD
       OR decision_tree_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    decision_tree_enable_hardening(decision_tree_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()
