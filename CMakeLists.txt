cmake_minimum_required(VERSION 3.0)

project(json)
set(LIB_SOURCES ${PROJECT_SOURCE_DIR}/src/json.cpp) 
set(LIB_HEADERS ${PROJECT_SOURCE_DIR}/include/json.h)
set(FCGI_SOURCES ${PROJECT_SOURCE_DIR}/src/webServer.cpp  ${PROJECT_SOURCE_DIR}/src/fileManager.cpp ${PROJECT_SOURCE_DIR}/src/main.cpp)
set(FCGI_HEADERS ${PROJECT_SOURCE_DIR}/include/webServer.h  ${PROJECT_SOURCE_DIR}/include/fileManager.h)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(json STATIC ${LIB_SOURCES} ${LIB_HEADERS})

add_executable(main ${PROJECT_SOURCE_DIR}/src/main.cpp ${LIB_HEADERS})
target_link_libraries(main json)

add_executable(fcgi ${FCGI_SOURCES} ${FCGI_HEADERS} ${LIB_HEADERS})
target_link_libraries(fcgi json fastcgipp)
set_target_properties(fcgi PROPERTIES OUTPUT_NAME script.fcgi)

