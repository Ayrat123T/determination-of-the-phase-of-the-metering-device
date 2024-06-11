cmake_minimum_required(VERSION 3.11)

project(phasing CXX)
set(CMAKE_CXX_STANDARD 20)

set(SOURCE_DIR src)
set(SOURCES_FILES ${SOURCE_DIR}/main.cpp ${SOURCE_DIR}/electricity_meter.cpp)
set(HEAD_FILES ${SOURCE_DIR}/meters_data.h ${SOURCE_DIR}/electricity_meter.h)

add_executable(${PROJECT_NAME} ${SOURCES_FILES} ${HEAD_FILES})

target_link_libraries (${PROJECT_NAME} PRIVATE libpq) #ниже приведена линковка libpq

set(libpq_DIR "C:/Program Files/PostgreSQL/15") #каталог с установленной PostgreSQL
set(libpq_LIB_DIR ${libpq_DIR}/lib)
set(libpq_INCLUDE_DIR ${libpq_DIR}/include)
add_library(libpq STATIC IMPORTED)
set_property(TARGET libpq PROPERTY IMPORTED_LOCATION ${libpq_LIB_DIR}/libpq.lib)
target_include_directories(libpq INTERFACE ${libpq_INCLUDE_DIR})
#
if(CMAKE_SYSTEM_NAME MATCHES "^MINGW")
    set(SYSTEM_LIBS -lstdc++)
else()
    set(SYSTEM_LIBS)
endif() 

target_link_libraries(${PROJECT_NAME} ${SYSTEM_LIBS})