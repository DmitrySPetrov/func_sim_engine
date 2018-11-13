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

#ifndef __LIBKMS_DETAIL_SPEC_HPP__
#define __LIBKMS_DETAIL_SPEC_HPP__


#include <detail/remappable_map.hpp>

#include <map>
#include <memory>


namespace LIBKMS_namespace {

//!	Определение типов на основе стандартных умного указателя и словаря
template< typename T >
struct shared_ptr_spec {
	//!	Указатель на объект
	typedef std::shared_ptr< T > ptr;
	//!	Константный указатель
	typedef const std::shared_ptr< T > const_ptr;
	//!	Метод для удаления указателя
	static void deleter( ptr & ) {}
	//!	Пустой указатель
	static ptr null_ptr() { return ptr(); }

	//!	Словарь указателей
	typedef std::map< std::string, ptr > map;

	//!	Приведение указателей
	template< typename U, typename V >
	static std::shared_ptr< U >
		dynamic_ptr_cast( const std::shared_ptr< V > & sp )
	{
		return std::dynamic_pointer_cast< U >( sp );
	}
};

//!	Определние типов на основе стандартного умного указателя и словаря с
//	возможностью подмены ключей
template< typename T >
struct remappable_shared_ptr_spec: shared_ptr_spec< T > {
	//!	Словарь указателей с возможностью подмены ключей
	typedef ::remappable_map<
			std::string,
			typename shared_ptr_spec< T >::ptr,
			std::map,
			std::map,
			std::shared_ptr
		> map;
};


}


#endif
