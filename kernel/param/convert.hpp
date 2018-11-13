////////////////////////////////////////////////////////////////////////////////
// 
//  This file is part of Functional Simulation Engine.
//
//  Functional Simulation Engine is free software: you can redistribute it
//  and/or modify it under the terms of the GNU General Public License as
//  published by the Free Software Foundation, either version 3 of the License,
//  or (at your option) any later version.
//
//  Functional Simulation Engine is distributed in the hope that it will be
//  useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
//  Public License for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with Functional Simulation Engine.
//  If not, see <https://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __LIBKMS_PARAM_CONVERT_HPP__
#define __LIBKMS_PARAM_CONVERT_HPP__


#include <detail/namespace.hpp>
#include <param/fwd.hpp>


namespace LIBKMS_namespace {


//!	Шаблонный класс для преобразования параметров в нужный тип
template< typename T, typename P >
struct obj_as {

static T & as( P & obj ) {
	return dynamic_cast< T & >( obj );
}

static const T & c_as( const P & obj ) {
	return dynamic_cast< const T & >( obj );
}

};	//	template< typename T > struct obj_as


}	//	namespace LIBKMS_namespace


#endif
