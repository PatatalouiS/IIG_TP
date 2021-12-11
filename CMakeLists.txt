project(IIG3D-BaseTP)

############################################
# Configure CMake and GCC flags
cmake_minimum_required(VERSION 2.8.12) # Minimal version compatible QT5
CMAKE_POLICY(SET CMP0043 NEW) # This will silence the Cmake Warning "Policy CMP0043 is not set"

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -pedantic")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "/usr/local/homebrew/lib/cmake/assimp-5.1")

IF(POLICY CMP0012)
CMAKE_POLICY(SET CMP0012 NEW)
ENDIF()


find_package(Qt5 COMPONENTS OpenGL REQUIRED)
if (Qt5OpenGL_FOUND)
    message(STATUS "Found Qt ${Qt5OpenGL_VERSION}")
else (Qt5OpenGL_FOUND)
    message(EROR "Unable to find QTOpenGL library")
endif(Qt5OpenGL_FOUND)

find_package(OpenGL REQUIRED) # define OPENGL_LIBRARIES

find_package(ASSIMP REQUIRED)

#OpenGL deprecated on MacosX : remove compilation warning
add_definitions(-DGL_SILENCE_DEPRECATION)

################################################################################
# Define project private sources and headers
#
# the variable "folder_source" contains all .cpp files of this project
FILE(GLOB_RECURSE
    folder_source
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

FILE(GLOB_RECURSE
    folder_header
    ${CMAKE_SOURCE_DIR}/src/*.h

)

FILE(GLOB_RECURSE
    folder_ui
    ${CMAKE_SOURCE_DIR}/src/*.ui
)

FILE(GLOB_RECURSE
	folder_shader
	${CMAKE_SOURCE_DIR}/shaders/*.vert
	${CMAKE_SOURCE_DIR}/shaders/*.frag
	${CMAKE_SOURCE_DIR}/shaders/*.tesc
	${CMAKE_SOURCE_DIR}/shaders/*.tese
	${CMAKE_SOURCE_DIR}/shaders/*.glsl
)

include_directories(
   ${CMAKE_SOURCE_DIR}/src
   ${CMAKE_SOURCE_DIR}/glm
)

message(STATUS ${ASSIMP_LIBRARY_DIRS})

SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})
SOURCE_GROUP("Shader Files" FILES ${folder_shader})

################################################################################
# Configure QT

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set( CMAKE_INCLUDE_CURRENT_DIR ON )


################################################################################
# Build target application

add_executable(${PROJECT_NAME}
               ${folder_source}
               ${folder_header}
			   ${folder_shader}
               ${folder_ui}
               )

set(EXT_LIBS Qt5::OpenGL ${OPENGL_LIBRARIES} ${ASSIMP_LIBRARIES})

target_link_libraries(${PROJECT_NAME} ${EXT_LIBS})
