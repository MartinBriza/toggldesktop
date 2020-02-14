include_directories(
    ${NEXTLIBRARY_INCLUDE_DIR}
    ${GTEST_INCLUDE_DIRS}
)

set(MEMORY_TEST_SOURCE_FILES
    memory_test.cc
)

add_executable(test_memory ${MEMORY_TEST_SOURCE_FILES})
target_link_libraries(test_memory
    NextLibrary
    gtest gtest_main
)
