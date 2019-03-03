cmake_minimum_required(VERSION 2.6)
project(ble-temperature-sensor)

find_package(PkgConfig REQUIRED)

pkg_search_module(GATTLIB REQUIRED gattlib)
pkg_search_module(GLIB REQUIRED glib-2.0)
pkg_search_module(PCRE REQUIRED libpcre)

include_directories(${GLIB_INCLUDE_DIRS})

set(temperature_SRCS main.c)

add_executable(temperature ${temperature_SRCS})
target_link_libraries(temperature ${GATTLIB_LIBRARIES} ${GATTLIB_LDFLAGS} ${GLIB_LDFLAGS} ${PCRE_LIBRARIES} pthread)
