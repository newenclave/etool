cmake_minimum_required( VERSION 2.8 )

set( PROJECT_NAME etool_test )

project( ${PROJECT_NAME} )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED 11)

list( APPEND src . )

foreach( dir ${src} )
    aux_source_directory( ${dir} src_dirs )
endforeach(dir)

add_executable( ${PROJECT_NAME} ${src_dirs} )

include_directories(  )
