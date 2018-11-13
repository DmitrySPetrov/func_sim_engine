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

#ifndef __LIBKMS_TYPEFACTORY_HPP__
#define __LIBKMS_TYPEFACTORY_HPP__

#include <detail/fwd.hpp>

#include <boost/functional/factory.hpp>
#include <boost/function.hpp>

#include <map>


namespace LIBKMS_namespace {


//!	Фабрика параметров
//	позволяет создавать объекты по типу, указанному в виде строки
template<
	typename Ext,
	template< typename U > class Spec
>
class TypeFactory {

	template< typename T >
	struct tFactory:
		boost::factory< typename Spec< T >::ptr >
		{};

public:

	typedef TypeFactory< Ext, Spec > type_factory_t;
	typedef Parameter< Ext, Spec > parameter_t;
	typedef typename parameter_t::ptr_t parameter_ptr;

	typedef Holder< std_types::tBool, parameter_t > bool_holder;
	typedef Holder< std_types::tInt, parameter_t > int_holder;
	typedef Holder< std_types::tReal, parameter_t > real_holder;
	typedef Holder< std_types::tString, parameter_t > string_holder;

public:

	//	Возвращаем ссылку на синглтон
	static type_factory_t & instance() noexcept {
		static type_factory_t instance;
		return instance;
	}

	//	Создаем экземпляр класса с указанным именем
	parameter_ptr create( const std::string & name ) const noexcept {
		auto _f = _factory.find( name );
		if( _f == _factory.end() )
			{ return parameter_t::null_ptr(); }
		return _f->second();
	}

	//	Добавить новый тип
	template< typename T >
	void add_type( const std::string & name ) {
		_factory[ name ] = tFactory< T >();
	}

private:

	//	Инициализация
	TypeFactory() {
		//	Заполняем фабрику стандартными типами
		_factory[ std_types::KMS_BOOL_TYPE_NAME ]
			= tFactory< bool_holder >();
		_factory[ std_types::KMS_INT_TYPE_NAME ]
			= tFactory< int_holder >();
		_factory[ std_types::KMS_REAL_TYPE_NAME ]
			= tFactory< real_holder >();
		_factory[ std_types::KMS_STRING_TYPE_NAME ]
			= tFactory< string_holder >();
	};
	TypeFactory( const type_factory_t & );
	TypeFactory & operator=( const type_factory_t & );

	typedef boost::function< parameter_ptr() > p_factory;
	std::map< std::string, p_factory > _factory;
};	//	class TypeFactory


}	//	namespace LIBKMS_namespace


#endif

