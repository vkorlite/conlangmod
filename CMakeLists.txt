cmake_minimum_required(VERSION 3.23)

project(conlangmod_vcb_test)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

add_executable(clm_vcb_test)

target_sources(clm_vcb_test
    PRIVATE
        tests/vcb_test.cpp
)

add_library(clm_vcb_library)

target_sources(clm_vcb_library
    PRIVATE
        src/st/st_string.cpp
        src/st/st_Exception.cpp
        src/vcb/var.cpp
        src/vcb/st_Object.cpp
        src/vcb/condition.cpp
        src/vcb/domain.cpp
        src/vcb/com.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/st/st_string.h
            include/st/st_Exception.h
            include/vcb/var.h
            include/vcb/st_Object.h
            include/vcb/condition.h
            include/vcb/domain.h
            include/vcb/com.h
)

target_link_libraries(clm_vcb_test
    PRIVATE
        clm_vcb_library
)
