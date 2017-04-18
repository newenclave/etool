cmake_minimum_required( VERSION 2.8 )

set( PROJECT_NAME etool_test )

project( ${PROJECT_NAME} )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED 11)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/catch )

list( APPEND src ${CMAKE_CURRENT_SOURCE_DIR}/tests
                 ${CMAKE_CURRENT_SOURCE_DIR}/cache
                 ${CMAKE_CURRENT_SOURCE_DIR}/cache/traits
                 ${CMAKE_CURRENT_SOURCE_DIR}/details
                 ${CMAKE_CURRENT_SOURCE_DIR}/dumper
                 ${CMAKE_CURRENT_SOURCE_DIR}/logger
                 ${CMAKE_CURRENT_SOURCE_DIR}/observers/traits/
                 ${CMAKE_CURRENT_SOURCE_DIR}/observers
                 ${CMAKE_CURRENT_SOURCE_DIR}/queues/condition/traits
                 ${CMAKE_CURRENT_SOURCE_DIR}/queues/condition
                 ${CMAKE_CURRENT_SOURCE_DIR}/sizepack
                 ${CMAKE_CURRENT_SOURCE_DIR}/cache/traits
                 ${CMAKE_CURRENT_SOURCE_DIR}/cache
                 ${CMAKE_CURRENT_SOURCE_DIR}/trees/trie/nodes
                 ${CMAKE_CURRENT_SOURCE_DIR}/trees/trie
                 ${CMAKE_CURRENT_SOURCE_DIR}/intervals
                 ${CMAKE_CURRENT_SOURCE_DIR}/intervals/traits
                 ${CMAKE_CURRENT_SOURCE_DIR}/slices
                 ${CMAKE_CURRENT_SOURCE_DIR}/catch
                 )

foreach( dir ${src} )
    aux_source_directory( ${dir} src_dirs )
    file(GLOB headers ${src_dir}/*.h)
    list(APPEND lib_src ${headers})
endforeach(dir)

execute_process( COMMAND "git" "submodule" "init"
                 WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

execute_process( COMMAND "git" "submodule" "update" "catch"
                 WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable( ${PROJECT_NAME} ${src_dirs} )

