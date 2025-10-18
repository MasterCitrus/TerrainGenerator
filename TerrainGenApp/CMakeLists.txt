cmake_minimum_required(VERSION 3.10)

add_executable(TerrainGenApp icon.rc)

file(GLOB_RECURSE EXE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
file(GLOB_RECURSE EXE_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/ PREFIX "Source Files" FILES ${EXE_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/ PREFIX "Header Files" FILES ${EXE_HEADERS})

target_sources(TerrainGenApp PRIVATE ${EXE_SOURCES} ${EXE_HEADERS})

target_include_directories(TerrainGenApp
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(TerrainGenApp
    PRIVATE
        TerrainGenLib
	PUBLIC
		glad
		SDL3::SDL3
		imgui
		stb
		glm
)