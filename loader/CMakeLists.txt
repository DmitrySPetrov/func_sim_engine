project( kmsloader )

cmake_minimum_required( VERSION 2.6 )


# Проверяем, есть ли c++11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
	message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()


# Проверяем, есть ли boost
find_package( Boost REQUIRED COMPONENTS date_time filesystem program_options system thread )
include_directories( ${Boost_INCLUDE_DIR} )


set( kmsloader_SRC loader_opts.cpp )

include_directories( . )


# Добавляем заголовочные файлы YAML
include_directories( ${YAML_CPP_SOURCE_DIR}/include )


# Добавляем заголовочные файлы libkms
include_directories( ${libkms_SOURCE_DIR} )


# Дополнительные библиотеки
if(UNIX)
	set( additional_libs dl rt )
endif(UNIX)

add_library( loader STATIC ${kmsloader_SRC} )
target_link_libraries( loader ${Boost_LIBRARIES} kms yaml-cpp ${additional_libs} )
