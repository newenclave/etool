cmake_minimum_required( VERSION 2.8 )

set( PROJECT_NAME etool_test )

project( ${PROJECT_NAME} )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED 11)

include_directories( ./ )
list( APPEND src . )
list( APPEND src ./tests )

foreach( dir ${src} )
    aux_source_directory( ${dir} src_dirs )
endforeach(dir)

execute_process( COMMAND "git" "submodule" "init"
                 WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

execute_process( COMMAND "git" "submodule" "update" "catch"
                 WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable( ${PROJECT_NAME} ${src_dirs} )

