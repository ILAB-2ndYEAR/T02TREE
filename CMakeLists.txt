
cmake_minimum_required( VERSION 3.11 )

project( Tree VERSION 0.1 LANGUAGES CXX )
find_package( GTest REQUIRED )

set( TREE_EXEC_NAME "tree" )
set( TREE_TEST_NAME "treeTest" )
set( TREE_E2E_NAME "treeE2E" )

set( TARGETS
    ${TREE_EXEC_NAME}
    ${TREE_TEST_NAME}
    ${TREE_E2E_NAME}
)

foreach( TARGET IN LISTS TARGETS )

    add_executable( ${TARGET} )
    target_compile_features( ${TARGET} PRIVATE cxx_std_17 )
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
set( TREE_TESTS_DIR "${CMAKE_SOURCE_DIR}/source/tree-tests" )
set( TREE_E2E_DIR "${CMAKE_SOURCE_DIR}/source/e2e")

target_sources( ${TREE_EXEC_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/source/tree-main.cc")

target_sources( ${TREE_TEST_NAME} PRIVATE "${TREE_TESTS_DIR}/tests-main.cc" )
target_sources( ${TREE_E2E_NAME} PRIVATE "${TREE_E2E_DIR}/e2e-main.cc" )

set( DEBUG_DEFINE_NAME "DEBUG" )

set( DEBUG_COMPILER_FLAGS
    -D${DEBUG_DEFINE_NAME}=1 -O0 -g -Wall -Wextra -Wpedantic
)

set( RELEASE_COMPILER_FLAGS
    -D${DEBUG_DEFINE_NAME}=0 -O2
)

target_compile_options( ${TREE_EXEC_NAME} PRIVATE ${RELEASE_COMPILER_FLAGS} )
target_compile_options( ${TREE_TEST_NAME} PRIVATE ${DEBUG_COMPILER_FLAGS} )
target_compile_options( ${TREE_E2E_NAME} PRIVATE ${DEBUG_COMPILER_FLAGS} )
