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

#ifndef __LIBKMS_SUBPROG_FACTORY_HPP__
#define __LIBKMS_SUBPROG_FACTORY_HPP__


#include <map>
#include <string>
#include <functional>


namespace LIBKMS_namespace {


//!	Фабрика подпрограмм
template< typename ParameterMap >
class FunctionStorage {

public:
	typedef FunctionStorage< ParameterMap > function_storage_t;

	typedef ParameterMap parameter_map;

	typedef std::function< parameter_map(
			double, double, const parameter_map &, const parameter_map & )
		> function_t;

public:
	//!	Возвращает ссылку на объект синглтона
	static function_storage_t & instance() noexcept
	{
		static function_storage_t instance;
		return instance;
	}

	//!	Создает экземпляр подпрограммы
	//const function_t & operator[]( std::string name ) const noexcept;

	//!	Проверяет наличие подпрограммы с данным именем и возвращает указатель
	//!	на нее
	//!	\return Указатель на подпрограмму, либо нуль, если такого имени не
	//!		зарегистрировано
	const function_t * get( std::string name ) const noexcept
	{
		auto res = _storage.find( name );
		if( res != _storage.end() ) {
			return &( res->second );
		} else {
			return 0;
		}
	}


	//!	Добавляет новую подпрограмму
	//!	\return добавилась ли функция
	bool add_func( std::string name, const function_t & f ) noexcept
	{
		//	Используем insert чтобы не затереть имеющиеся данные
		return _storage.insert( make_pair( name, f ) ).second;
	}

private:

	FunctionStorage() {
		_do_nothing = function_storage_t::do_nothing;
	}

	FunctionStorage( const function_storage_t & );
	FunctionStorage & operator=( const function_storage_t & );

	function_t _do_nothing;

	static parameter_map do_nothing
		( double, double, const parameter_map &, const parameter_map & )
	{
		parameter_map res;
		return res;
	}

	std::map< std::string, function_t > _storage;
};


}	//	namespace LIBKMS_namespace


#endif
