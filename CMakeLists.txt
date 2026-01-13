cmake_minimum_required(VERSION 3.23)

project(conlangmod_test)
add_compile_options(-Wall -Werror)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(SQLite3 REQUIRED)

add_executable(clm_test)

target_sources(clm_test
    PRIVATE
        # tests/vcb_test.c
        tests/clm_test.cpp
)


add_library(clm_vcb_library)

target_sources(clm_vcb_library
    PRIVATE
        src/vcb/arena.c
        src/vcb/st.c
        src/vcb/list.c
        src/vcb/hashtable.c
        src/vcb/system_hash.c
        src/vcb/cond.c
        src/vcb/var.c
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/vcb/vcb.h
)

add_library(clm_library)

target_sources(clm_library
    PRIVATE
        src/clm/phonetics.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/clm/clm.h
)

target_link_libraries(clm_test
    PRIVATE
        clm_vcb_library
        clm_library
        SQLite::SQLite3
)
