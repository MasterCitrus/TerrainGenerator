cmake_minimum_required(VERSION 3.10)
project(TerrainGeneratorLibrary)

#-----------------------------
#  Global Settings
#-----------------------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

#-----------------------------
#  Options
#-----------------------------



#-----------------------------
# Includes
#-----------------------------

# Dependencies
add_subdirectory(dep)

# Libray Projects
add_subdirectory(TerrainGenLib)
add_subdirectory(TerrainGenApp)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT TerrainGenApp)