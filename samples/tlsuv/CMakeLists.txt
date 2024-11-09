if (MSVC)
    find_package(unofficial-getopt-win32 REQUIRED)
endif()

set_source_files_properties(common.c http-get.c PROPERTIES LANGUAGE CXX)

add_library(tlsuv_common OBJECT common.c)
target_link_libraries(tlsuv_common PUBLIC awaittlsuv)
target_compile_definitions(tlsuv_common PUBLIC TLSUV_HTTP)

add_executable(tlsuv_http-get http-get.c)
target_link_libraries(tlsuv_http-get PUBLIC awaittlsuv tlsuv_common)

add_executable(tlsuv_http-post http-post.cpp)
target_link_libraries(tlsuv_http-post PUBLIC awaittlsuv tlsuv_common)