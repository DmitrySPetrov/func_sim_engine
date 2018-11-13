#ifndef __LIBKMS_CONTROL_FUNC_VARS_HPP__
#define __LIBKMS_CONTROL_FUNC_VARS_HPP__

#include <control_func.hpp>

namespace LIBKMS_namespace {

namespace std_fi {


typedef StdStringSpec StringSpec;


//	ФИ инфп
//	получить значение переменной
template<
	typename TF,
	typename BP,
	typename L
>
class GetValue: public ControlFunc< TF, BP, L >
{
	typedef ControlFunc< TF, BP, L > control_func_t;
	typedef typename control_func_t::block_ptr block_ptr;

public:
	GetValue( std::string name ):
		control_func_t( name )
	{
		control_func_t::add_param( "строка" );	//	Имя переменной
	}

	std::string run( block_ptr main_block ){
		std::stringstream res;

		std::string name = control_func_t::get_param( 0 )->to_string();
		auto p = main_block->get_variable( name );
		if( p ) {
			res << StringSpec::print_var_info( name, p );
		} else {
			res << "Нет параметра \"" << name << "\"" << std::endl;
		}
		return res.str();
	}
};


//	ФИ устп
//	установить значение переменной
template<
	typename TF,
	typename BP,
	typename L
>
class SetValue: public ControlFunc< TF, BP, L >
{
	typedef ControlFunc< TF, BP, L > control_func_t;
	typedef typename control_func_t::block_ptr block_ptr;

public:
	SetValue( std::string name ):
		control_func_t( name )
	{
		control_func_t::add_param( "строка" );	//	Имя переменной
		control_func_t::add_param( "строка" );	//	Значение переменной
	}

	std::string run( block_ptr main_block ){
		std::stringstream res;

		std::string name = control_func_t::get_param( 0 )->to_string();
		std::string val = control_func_t::get_param( 1 )->to_string();
		auto p = main_block->get_variable( name );
		if( p ) {
			if( !p->from_string( val ) ) {
				res << "Неверное значение \"" << val << "\" для переменной "
					<< name << " типа " << p->get_type() << std::endl;
			} else {
				res << StringSpec::print_var_info( name, p );
			}
		} else {
			res << "Нет параметра \"" << name << "\"" << std::endl;
		}
		return res.str();
	}
};


//	ФИ блп
//	установить блокировку переменной
template<
	typename TF,
	typename BP,
	typename L
>
class LockParam: public ControlFunc< TF, BP, L >
{
	typedef ControlFunc< TF, BP, L > control_func_t;
	typedef typename control_func_t::block_ptr block_ptr;

public:
	LockParam( std::string name ):
		control_func_t( name )
	{
		control_func_t::add_param( "строка" );	//	Имя переменной
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

		auto p = main_block->get_variable( name );
		if( p ) {
			if( lock != p->is_locked() ) {
				p->set_lock( lock );
				res << StringSpec::print_var_info( name, p );
			}
		} else {
			res << "Нет параметра \"" << name << "\"" << std::endl;
		}
		return res.str();
	}
};


//	ФИ инфпблп
//	вывести список переменных блока
template<
	typename TF,
	typename BP,
	typename L
>
class GetVarList: public ControlFunc< TF, BP, L >
{
	typedef ControlFunc< TF, BP, L > control_func_t;
	typedef typename control_func_t::block_ptr block_ptr;

public:
	GetVarList( std::string name ):
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
			auto vars = p->get_var_list( false );
			for( auto & v: vars ) {
				res << StringSpec::print_var_info( v, p->get_variable( v ) );
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
