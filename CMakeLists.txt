#####################################################################
# Скрипт сборки исполняющей системы
#####################################################################
# Для сборки под Windows укажите параметр BOOST_ROOT
#####################################################################

cmake_minimum_required( VERSION 2.8 )

project( kms2 )

add_definitions( -DBOOST_ASIO_HAS_MOVE )

# Проверяем, есть ли c++11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
	message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()


# Проверяем, есть ли boost
find_package( Boost REQUIRED COMPONENTS date_time filesystem locale program_options system thread chrono )
include_directories( ${Boost_INCLUDE_DIR} )


find_package( yaml-cpp )


# Включаем все варнинги
if( MSVC )
	# Force to always compile with W4
	if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	endif()
elseif( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )
	# Update if necessary
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic -pthread")
endif()


include_directories( ${yaml-cpp_DIR}/include )

add_subdirectory( kernel )

include_directories( ${libkms_SOURCE_DIR} )

add_subdirectory( loader )

include_directories( ${kmsloader_SOURCE_DIR} )

add_subdirectory( process )

include_directories( ${libkmsproc_SOURCE_DIR} )

add_executable( run main.cpp )


target_link_libraries( run kms ${yaml-cpp_LIBS} loader process ${Boost_LIBRARIES} )

