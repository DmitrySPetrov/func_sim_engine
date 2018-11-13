######################################################################
#
#   This file is part of Functional Simulation Engine.
#
#   Functional Simulation Engine is free software: you can redistribute it
#   and/or modify it under the terms of the GNU General Public License as
#   published by the Free Software Foundation, either version 3 of the License,
#   or (at your option) any later version.
# 
#   Functional Simulation Engine is distributed in the hope that it will be
#   useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
#   Public License for more details.
# 
#   You should have received a copy of the GNU General Public License along
#   with Functional Simulation Engine.
#   If not, see <https://www.gnu.org/licenses/>.
#
######################################################################

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
