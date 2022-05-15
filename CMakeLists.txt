cmake_minimum_required(VERSION 3.22)
project(Slimemaid)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-O2 -Wall -Werror -Wextra -Winit-self -Wfloat-equal -Wno-unused-parameter -lglfw -lvulkan -ldl -lpthread -lX11 -lXrandr -lXi")


list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake")

find_package(STBImage REQUIRED)
include_directories(SYSTEM ${STBI_INCLUDE_DIR})

find_package(TinyObjLoader REQUIRED)
include_directories(SYSTEM ${TOL_INCLUDE_DIR})

set(PROJECT_HEADERS_DIR ${PROJECT_SOURCE_DIR}/include)
set(PROJECT_SOURCES_DIR ${PROJECT_SOURCE_DIR}/source)

file(GLOB_RECURSE HEADERS ${PROJECT_HEADERS_DIR}/*.hpp)
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCES_DIR}/*.cpp)
include_directories(${PROJECT_HEADERS_DIR})

file(GLOB HEADERS)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${STB_INCLUDE_DIRS})
install(FILES ${HEADERS} DESTINATION include/${PROJECT_NAME})


# Add linting before compilation
# include(CppLint)
# ADD_STYLE_CHECK_TARGET(lint ${PROJECT_NAME} ${PROJECT_HEADERS_DIR} ${PROJECT_SOURCES_DIR})
# add_dependencies(${PROJECT_NAME} lint)

