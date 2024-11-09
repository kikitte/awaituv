# Example use of awaituv

add_executable (test1 test1.cpp)
target_link_libraries(test1 awaituv)

add_executable (test2 test2.cpp)
target_link_libraries(test2 awaituv )

add_executable (test3 test3.cpp)
target_link_libraries(test3 awaituv awaitcurl)

add_executable (test4 test4.cpp)
target_link_libraries(test4 awaituv)

add_executable (test5 test5.cpp)
target_link_libraries(test5 awaituv)

add_executable (test6 test6.cpp)
target_link_libraries(test6 awaituv)

add_executable (test7 test7.cpp)
target_link_libraries(test7 awaituv)

if (ENABLE_TLSUV)
    add_subdirectory(tlsuv)
endif ()