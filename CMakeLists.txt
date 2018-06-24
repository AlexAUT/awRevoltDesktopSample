cmake_minimum_required(VERSION 3.6)

project(awRevoltTest VERSION 0.0.1 DESCRIPTION "awRevolt engine test app")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")

set(AW_DESKTOP true)
set(AW_USE_OPENGL true)
add_definitions(-DAW_DESKTOP)
add_definitions(-DAW_USE_OPENGL)

set(TEST_FILES
    src/main.cpp
    src/testState.cpp
    src/testState.hpp
    src/assimpImporter.hpp
    src/assimpImporter.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${TEST_FILES})

# Find SFML
#set(SFML_DIR "${CMAKE_SOURCE_DIR}/extlibs/lib/cmake/SFML")
#find_package(SFML 2 REQUIRED  window system)

# Setup assimp, we need to create a target ourself
#FILE(GLOB ASSIMP_DIR "${CMAKE_SOURCE_DIR}/extlibs/lib/cmake/assimp*")
#find_package(ASSIMP REQUIRED)
#if (ASSIMP_FOUND)
#  set(ASSIMP_LIBRARY "assimp")
#  add_library(${ASSIMP_LIBRARY} SHARED IMPORTED)
#  set_target_properties(${ASSIMP_LIBRARY} PROPERTIES
#    IMPORTED_LOCATION "${ASSIMP_LIBRARY_DIRS}/libassimp.so")
#endif()

#jsoncpp
#set(jsoncpp_DIR "${CMAKE_SOURCE_DIR}/extlibs/lib64/cmake/jsoncpp")
#find_package(jsoncpp REQUIRED)

# Link everyhing
# Find SFML
set(SFML_DIR "${CMAKE_SOURCE_DIR}/../extlibs/lib/cmake/SFML")
find_package(SFML 2 REQUIRED  window system)
target_link_libraries(${PROJECT_NAME} sfml-window sfml-system)

# Setup assimp, we need to create a target ourself
FILE(GLOB ASSIMP_DIR "${CMAKE_SOURCE_DIR}/../extlibs/lib/cmake/assimp*")
find_package(ASSIMP REQUIRED)
if (ASSIMP_FOUND)
  set(ASSIMP_LIBRARY "assimp")
  add_library(${ASSIMP_LIBRARY} SHARED IMPORTED)
  set_target_properties(${ASSIMP_LIBRARY} PROPERTIES
    IMPORTED_LOCATION "${ASSIMP_LIBRARY_DIRS}/libassimp.so")
endif()
target_link_libraries(${PROJECT_NAME} ${ASSIMP_LIBRARY})
#jsoncpp
set(jsoncpp_DIR "${CMAKE_SOURCE_DIR}/../extlibs/lib64/cmake/jsoncpp")
find_package(jsoncpp REQUIRED)
target_link_libraries(${PROJECT_NAME} jsoncpp)

add_library(awRevolt SHARED IMPORTED) # or STATIC instead of SHARED
set_target_properties(awRevolt PROPERTIES
  IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../extlibs/lib64/libawRevolt.so"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/../extlibs/include/"
)

target_link_libraries(${PROJECT_NAME} awRevolt GL)
