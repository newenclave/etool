cmake_minimum_required( VERSION 2.8 )

set( PROJECT_NAME etool_test )

project( ${PROJECT_NAME} )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED 11)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/catch )

execute_process( COMMAND "git" "submodule" "init"
                 WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

execute_process( COMMAND "git" "submodule" "update" "catch"
                 WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

list( APPEND src ./include/etool/cache
                 ./include/etool/cache/traits
                 ./include/etool/details
                 ./include/etool/dumper
                 ./include/etool/logger
                 ./include/etool/observers
                 ./include/etool/observers/traits/
                 ./include/etool/queues/condition
                 ./include/etool/queues/condition/traits
                 ./include/etool/sizepack
                 ./include/etool/cache/traits
                 ./include/etool/cache
                 ./include/etool/trees/trie
                 ./include/etool/trees/trie/nodes
                 ./include/etool/intervals
                 ./include/etool/intervals/traits
                 ./include/etool/slices
                 ./include/etool/catch
                 ./tests
                 )

foreach( src_dir ${src} )
    aux_source_directory(${src_dir} lib_src)
    file(GLOB headers ${src_dir}/*.h)
    list(APPEND lib_src ${headers})
endforeach( )

add_executable( ${PROJECT_NAME} ${lib_src} )

