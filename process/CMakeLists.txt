project( libkmsproc )

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
find_package( Boost REQUIRED thread locale )
include_directories( ${Boost_INCLUDE_DIR} )


# Собираем библиотеку
# На данный момент нечего компилировать, все в заголовочных файлах

set( libkmsproc_SRC
		boost_clock.cpp
		boost_tcp_instruction_reader.cpp
		boost_signal_handler.cpp
	)

include_directories( . )

add_library( process STATIC ${libkmsproc_SRC} )
target_link_libraries( process ${Boost_LIBRARIES} kms ${additional_libs} )
