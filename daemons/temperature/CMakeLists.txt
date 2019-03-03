cmake_minimum_required(VERSION 2.6)
project(ble-temperature-sensor)

find_package(PkgConfig REQUIRED)

pkg_search_module(GATTLIB REQUIRED gattlib)
pkg_search_module(GLIB REQUIRED glib-2.0)
pkg_search_module(PCRE REQUIRED libpcre)

include_directories(${GLIB_INCLUDE_DIRS})

set(test_SRCS test.c)

add_executable(testi ${test_SRCS})
target_link_libraries(testi ${GATTLIB_LIBRARIES} ${GATTLIB_LDFLAGS} ${GLIB_LDFLAGS} ${PCRE_LIBRARIES} pthread)
