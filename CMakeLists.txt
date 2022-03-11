cmake_minimum_required(VERSION 3.0.0)
project(Game-Client VERSION 0.1.0)

include(CTest)
enable_testing()

set(
  SOURCES
    src/main.cpp
    src/socket.cpp
    src/client_func.cpp
    src/lexer.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

include_directories(
  ${PROJECT_SOURCE_DIR}/include
)

if(WIN32)
  target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
