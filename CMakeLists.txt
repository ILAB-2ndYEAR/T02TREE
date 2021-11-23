
cmake_minimum_required( VERSION 3.11 )

project( Tree VERSION 0.1 LANGUAGES CXX )
find_package( GTest REQUIRED )

set( TREE_EXEC_NAME "tree" )
set( TREE_TEST_NAME "treeTest" )
set( FORMATTER "clang-format" )

set( TARGETS
    ${TREE_EXEC_NAME}
    ${TREE_TEST_NAME}
)

foreach( TARGET IN LISTS TARGETS )
    add_executable( ${TARGET} )
    target_compile_features( ${TARGET} PRIVATE cxx_std_20 )
    target_include_directories( ${TARGET} PRIVATE
        "${CMAKE_SOURCE_DIR}/headers"
    )
endforeach()

target_include_directories( ${TREE_TEST_NAME} PRIVATE
    "${GTEST_INCLUDE_DIRS}"
)

target_link_libraries( ${TREE_TEST_NAME} PRIVATE
    "${GTEST_LIBRARIES}"
    "pthread"
)

set( TREE_IMPL_DIR "${CMAKE_SOURCE_DIR}/source/tree-impl" )
set( TREE_TESTS_DIR "${CMAKE_SOURCE_DIR}/source/tree-utests" )

set( TESTS_SOURCES
    "tree-tests.cc"
    "tests-main.cc"
    "tree-tester-impl.cc"
)

target_sources( ${TREE_EXEC_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/source/tree-main.cc")

foreach(SOURCE IN LISTS TESTS_SOURCES)
    target_sources( ${TREE_TEST_NAME} PRIVATE "${TREE_TESTS_DIR}/${SOURCE}" )
endforeach()

set( DEBUG_DEFINE_NAME "DEBUG" )

set( DEBUG_COMPILER_FLAGS
    -D${DEBUG_DEFINE_NAME}=1 -O0 -g -Wall -Wextra -Wpedantic -Werror
)

set( RELEASE_COMPILER_FLAGS
    -D${DEBUG_DEFINE_NAME}=0 -O2
)

target_compile_options( ${TREE_EXEC_NAME} PRIVATE ${DEBUG_COMPILER_FLAGS} )
target_compile_options( ${TREE_TEST_NAME} PRIVATE ${DEBUG_COMPILER_FLAGS} )

# Formatting
execute_process( COMMAND sh -c "${FORMATTER} ${CMAKE_SOURCE_DIR}/headers/* -i")
foreach(SOURCE IN LISTS TESTS_SOURCES)
    execute_process(COMMAND sh -c "${FORMATTER} ${TREE_TESTS_DIR}/${SOURCE} -i")
endforeach()

enable_testing()

add_test( NAME UnitTests COMMAND
    ${TREE_TEST_NAME}
)
