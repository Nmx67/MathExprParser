cmake_minimum_required(VERSION 3.15)

#
# Project details
#

project(
  mep_lib
  DESCRIPTION  "Mathematical expression parser lib"
  VERSION 0.1.0
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MEP_LIB_SRCS	
	mep/mep.cpp
    mep/math.cpp
    mep/lexer.cpp
	mep/parser.cpp
)

include(GNUInstallDirs)

# Find the fmt library
#find_package(fmt CONFIG REQUIRED)

option(BUILD_SHARED_LIBS "Cmake build type" ON)
option(BUILD_TEST_APP "Build our test app" ON)

if(NOT BUILD_SHARED_LIBS)
  add_library(mep_lib STATIC)
else()
  add_library(mep_lib SHARED)
endif()

set(MEP_LIB_SOURCE_DIR ${PROJECT_SOURCE_DIR}/mep)
set(MEP_LIB_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/mep)



target_sources(mep_lib
# source files get added as before
   PRIVATE ${MEP_LIB_SRCS}
    
# header files get added via FILE_SET
# the BASE_DIR should point to the same directory that would be used as
# the target_include_directory; it will be added to the include directories
# of the lib target and be used to determine the relative install path
  PUBLIC FILE_SET HEADERS
  BASE_DIRS ${MEP_LIB_SOURCE_DIR} FILES  ${MEP_LIB_SOURCE_DIR}/mep.hpp
)
set_target_properties(mep_lib PROPERTIES VERSION ${PROJECT_VERSION})
#set_target_properties(mep_lib PROPERTIES PUBLIC_HEADER include/mep.hpp)

# target_include_directories(mep_lib PRIVATE ./mep)


if (BUILD_SHARED_LIBS AND MSVC)
    target_compile_definitions(mep_lib PRIVATE "BUILD_MEP_AS_DLL")
endif()

# Link your library to fmt
#target_link_libraries(mep_lib PRIVATE fmt::fmt)


# Add include directories
target_include_directories(mep_lib PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>   # for headers when building
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>  # for client in install mode
)
# install command needs to reference the file set again
#install(TARGETS mep_lib FILE_SET HEADERS)
# Install the library and its headers

install(TARGETS mep_lib EXPORT mep_lib_targets
    FILE_SET HEADERS    
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"    
)
install(FILES $<TARGET_PDB_FILE:mep_lib> DESTINATION bin OPTIONAL)

# Generate and install *-targets.cmake 
install(EXPORT mep_lib_targets
        FILE mep_lib-targets.cmake
        NAMESPACE mep_lib::
        DESTINATION share/mep_lib)

# Generate the config file in the current binary dir (this ensures it's not placed directly in source)
file(WRITE 
    "${CMAKE_CURRENT_BINARY_DIR}/mep_lib-config.cmake"
    "include(CMakeFindDependencyMacro)\n"
    "find_dependency(fmt CONFIG REQUIRED)\n"
    "include(\"\${CMAKE_CURRENT_LIST_DIR}/mep_lib-targets.cmake\")\n"
)

# Install the generated config file
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/mep_lib-config.cmake"
    DESTINATION cmake
)

# Build our TEST
if(BUILD_TEST_APP) 
    add_executable(MepParser main.cpp)
    target_include_directories(MepParser PRIVATE ./mep)
    target_link_libraries(MepParser PRIVATE mep_lib)
endif()
