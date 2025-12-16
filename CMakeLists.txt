cmake_minimum_required(VERSION 3.23)

project(conlangmod_vcb_c_test C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(SQLite3 REQUIRED)

add_executable(clm_vcb_test)

target_sources(clm_vcb_test
    PRIVATE
        tests/vcb_test.c
)


add_library(clm_vcb_library)

target_sources(clm_vcb_library
    PRIVATE
        src/vcb_c/arena.c
        src/vcb_c/st.c
        src/vcb_c/list.c
        src/vcb_c/hashtable.c
        src/vcb_c/system_hash.c
        src/vcb_c/value.c
        src/vcb_c/cond.c
        src/vcb_c/var.c
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/vcb_c/vcb.h
)


# target_sources(clm_vcb_library
#     PRIVATE
#         src/st/st_string.cpp
#         src/st/st_Exception.cpp
#         src/vcb/var.cpp
#         src/vcb/st_Object.cpp
#         src/vcb/condition.cpp
#         src/vcb/domain.cpp
#         src/vcb/com.cpp
#     PUBLIC
#         FILE_SET HEADERS
#         BASE_DIRS
#             include
#         FILES
#             include/st/st_string.h
#             include/st/st_Exception.h
#             include/vcb/var.h
#             include/vcb/st_Object.h
#             include/vcb/condition.h
#             include/vcb/domain.h
#             include/vcb/com.h
# )

target_link_libraries(clm_vcb_test
    PRIVATE
        clm_vcb_library
        SQLite::SQLite3
)
