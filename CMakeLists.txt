cmake_minimum_required(VERSION 3.20)

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/manifest" release_version REGEX "^version: ")
if(NOT release_version MATCHES "^version: ([0-9]+\\.[0-9]+\\.[0-9]+)$")
  message(FATAL_ERROR "The CMake build requires a stable release version in manifest")
endif()
project(libstudxml VERSION "${CMAKE_MATCH_1}" LANGUAGES C CXX)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
  message(FATAL_ERROR "The libstudxml CMake build supports only Linux; use build2 on other platforms")
endif()

include(GNUInstallDirs)
find_package(EXPAT 2.1 REQUIRED)

set(LIBSTUDXML_ABI_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
math(EXPR libstudxml.version.project_number
  "${PROJECT_VERSION_MAJOR} * 100000000000000 + ${PROJECT_VERSION_MINOR} * 1000000000 + ${PROJECT_VERSION_PATCH} * 10000")
set(libstudxml.version.project "${PROJECT_VERSION}")
set(libstudxml.version.project_id "${PROJECT_VERSION}")
set(libstudxml.version.major "${PROJECT_VERSION_MAJOR}")
set(libstudxml.version.minor "${PROJECT_VERSION_MINOR}")
set(libstudxml.version.patch "${PROJECT_VERSION_PATCH}")
set(libstudxml.version.pre_release false)
set(libstudxml.version.snapshot_sn 0)
set(libstudxml.version.snapshot_id "")
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/libstudxml/version.hxx.in" version_header)
string(REGEX REPLACE "\\$([a-z_.]+)\\$" "@\\1@" version_header "${version_header}")
string(CONFIGURE "${version_header}" version_header @ONLY)
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/libstudxml")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/libstudxml/version.hxx" "${version_header}")

file(GLOB_RECURSE STUDXML_SOURCES CONFIGURE_DEPENDS "libstudxml/*.cxx")
add_library(studxml SHARED ${STUDXML_SOURCES}
  libstudxml/details/genx/genx.c
  libstudxml/details/genx/char-props.c)
set_target_properties(studxml PROPERTIES OUTPUT_NAME "studxml-${LIBSTUDXML_ABI_VERSION}")
target_compile_features(studxml PUBLIC cxx_std_11)
target_compile_definitions(studxml
  PUBLIC LIBSTUDXML_EXTERNAL_EXPAT
  PRIVATE LIBSTUDXML_SHARED_BUILD
  INTERFACE LIBSTUDXML_SHARED)
target_include_directories(studxml PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(studxml PUBLIC EXPAT::EXPAT)
add_custom_command(TARGET studxml POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E create_symlink
    "$<TARGET_FILE_NAME:studxml>" "$<TARGET_FILE_DIR:studxml>/libstudxml.so"
  VERBATIM)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  configure_file(libstudxml.pc.in libstudxml.pc @ONLY)
  install(TARGETS studxml LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
  install(FILES "$<TARGET_FILE_DIR:studxml>/libstudxml.so" DESTINATION ${CMAKE_INSTALL_LIBDIR})
  install(DIRECTORY libstudxml/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libstudxml
    FILES_MATCHING PATTERN "*.hxx" PATTERN "*.ixx" PATTERN "*.txx" PATTERN "*.h"
    PATTERN "version.hxx" EXCLUDE PATTERN "expat" EXCLUDE)
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libstudxml/version.hxx"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libstudxml)
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libstudxml.pc"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()
