cmake_minimum_required(VERSION 3.10)

add_library(TerrainGenLib)

file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE LIB_HEADERS CONFIGURE_DEPENDS include/*.h)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/ PREFIX "Source Files" FILES ${LIB_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/ PREFIX "Header Files" FILES ${LIB_HEADERS})

target_sources(TerrainGenLib
    PRIVATE
        ${LIB_SOURCES}
    PUBLIC
        $<BUILD_INTERFACE:${LIB_HEADERS}>
        $<INSTALL_INTERFACE:include>
)

target_include_directories(TerrainGenLib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(TerrainGenLib
    PRIVATE
        stb
		glm
		assimp
)