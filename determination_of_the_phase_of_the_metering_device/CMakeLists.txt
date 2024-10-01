cmake_minimum_required(VERSION 3.11)

project(metering_device_phase_determination CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

set(SOURCE_DIR src)
set(SOURCES_FILES ${SOURCE_DIR}/main.cpp   ${SOURCE_DIR}/electricity_meter.cpp ${SOURCE_DIR}/input_reader.cpp)
set(HEAD_FILES ${SOURCE_DIR}/meters_data.h ${SOURCE_DIR}/electricity_meter.h   ${SOURCE_DIR}/input_reader.h)

add_executable(${PROJECT_NAME} ${SOURCES_FILES} ${HEAD_FILES})

#линковка libpq через vcpkg
find_package(PostgreSQL REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE PostgreSQL::PostgreSQL)

#линковка libpq
#[===[
target_link_libraries (${PROJECT_NAME} PRIVATE libpq) 

set(libpq_DIR "C:/Program Files/PostgreSQL/15") #каталог с установленной PostgreSQL
set(libpq_LIB_DIR ${libpq_DIR}/lib)
set(libpq_INCLUDE_DIR ${libpq_DIR}/include)
add_library(libpq STATIC IMPORTED)
set_property(TARGET libpq PROPERTY IMPORTED_LOCATION ${libpq_LIB_DIR}/libpq.lib)
target_include_directories(libpq INTERFACE ${libpq_INCLUDE_DIR})
]===]

#add_library(libpq STATIC IMPORTED)
#set_property(TARGET libpq PROPERTY IMPORTED_LOCATION ${SOURCE_DIR}/libpq.lib)
#target_include_directories(libpq INTERFACE ${SOURCE_DIR})

if(CMAKE_SYSTEM_NAME MATCHES "^MINGW")
    set(SYSTEM_LIBS -lstdc++)
else()
    set(SYSTEM_LIBS)
endif() 

target_link_libraries(${PROJECT_NAME} ${SYSTEM_LIBS})