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

#ifndef __LIBKMS_CONTROL_FUNC_SUBPROG_HPP__
#define __LIBKMS_CONTROL_FUNC_SUBPROG_HPP__

#include <control_func.hpp>

namespace LIBKMS_namespace {

namespace std_fi {


typedef StdStringSpec StringSpec;


//	ФИ блпп
//	заблокировать подпрограмму
template<
	typename TF,
	typename BP,
	typename L
>
class LockSubprog: public ControlFunc< TF, BP, L >
{
	typedef ControlFunc< TF, BP, L > control_func_t;
	typedef typename control_func_t::block_ptr block_ptr;

public:
	LockSubprog( std::string name ):
		control_func_t( name )
	{
		control_func_t::add_param( "строка" );	//	Имя блока
		control_func_t::add_param( "строка" );	//	Признак блокировки
	}

	std::string run( block_ptr main_block ){
		std::stringstream res;

		std::string name = control_func_t::get_param( 0 )->to_string();
		std::string s_lock = control_func_t::get_param( 1 )->to_string();

		bool lock = false;
		if( s_lock == "вкл" ) {
			lock = true;
		} else if( s_lock != "выкл" ) {
			res << "Неверный признак блокировки \"" << s_lock
				<< "\" укажите \"вкл\" или \"выкл\"" << std::endl;
		}

		auto b = main_block->get_subprog( name );
		if( b ) {
			b->set_lock( lock );
			res << ( lock ? "Установлена":"Снята" )
				<< " блокировка подпрограммы \"" << name << "\"" << std::endl;
		} else {
			res << "Нет подпрограммы \"" << name << "\"" << std::endl;
		}
		return res.str();
	}
};


//	ФИ инфппблп
//	вывести список подпрограмм блока
template<
	typename TF,
	typename BP,
	typename L
>
class GetSubpList: public ControlFunc< TF, BP, L >
{
	typedef ControlFunc< TF, BP, L > control_func_t;
	typedef typename control_func_t::block_ptr block_ptr;

public:
	GetSubpList( std::string name ):
		control_func_t( name )
	{
		control_func_t::add_param( "строка" );	//	Имя блока
		control_func_t::add_param( "строка" );	//	Фильтр переменных
	}

	std::string run( block_ptr main_block ){
		std::stringstream res;

		std::string block = control_func_t::get_param( 0 )->to_string();
		std::string filter = control_func_t::get_param( 1 )->to_string();

		auto p = main_block->get_child( block );
		if( p ) {
			auto subs = p->get_subp_list( false );
			for( auto & s: subs ) {
				res << StringSpec::print_subp_info( s, p->get_subprog( s ) );
			}
		} else {
			res << "Нет блока \"" << block << "\"" << std::endl;
		}
		return res.str();
	}
};


}	//	namespace std_fi

}	//	namespace LIBKMS_namespace

#endif
