cmake_minimum_required(VERSION 3.22)
project(Slimemaid)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(STBImage REQUIRED)
include_directories(SYSTEM ${STBI_INCLUDE_DIR})

find_package(TinyObjLoader REQUIRED)
include_directories(SYSTEM ${TOL_INCLUDE_DIR})


set(LDFLAGS "-lglfw -lvulkan -ldl -lpthread -lX11 -lXrandr -lXi")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -pedantic -Wzero-as-null-pointer-constant -Winit-self -Wswitch-default -Wfloat-equal -Wno-unused-parameter ${LDFLAGS} -fPIC -march=native -I${STBI_INCLUDE_DIR} -I${TOL_INCLUDE_DIR}")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(PROJECT_HEADERS_DIR ${PROJECT_SOURCE_DIR}/include)
set(PROJECT_SOURCES_DIR ${PROJECT_SOURCE_DIR}/source)

file(GLOB_RECURSE HEADERS ${PROJECT_HEADERS_DIR}/*.hpp)
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCES_DIR}/*.cpp)
include_directories(${PROJECT_HEADERS_DIR})



file(GLOB HEADERS)

add_subdirectory(raws)
add_subdirectory(shaders)
add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
# target_link_libraries(${PROJECT_NAME} ${})
# file(COPY ${} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
install(FILES ${HEADERS} DESTINATION include/${PROJECT_NAME})


# Add linting before compilation
# include(CppLint)
# ADD_STYLE_CHECK_TARGET(lint ${PROJECT_NAME} ${PROJECT_HEADERS_DIR} ${PROJECT_SOURCES_DIR})
# add_dependencies(${PROJECT_NAME} lint)
