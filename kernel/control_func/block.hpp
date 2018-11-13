#ifndef __LIBKMS_CONTROL_FUNC_BLOCK_HPP__
#define __LIBKMS_CONTROL_FUNC_BLOCK_HPP__


#include <control_func.hpp>

namespace LIBKMS_namespace {

namespace std_fi {


//	ФИ блблп
//	заблокировать блок
template<
	typename TF,
	typename BP,
	typename L
>
class LockBlock: public ControlFunc< TF, BP, L >
{
	typedef ControlFunc< TF, BP, L > control_func_t;
	typedef typename control_func_t::block_ptr block_ptr;

public:
	LockBlock( std::string name ):
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

		auto b = main_block->get_child( name );
		if( b ) {
			b->set_lock( lock );
			res << ( lock ? "Установлена":"Снята" ) << " блокировка блока \""
				<< name << "\"" << std::endl;
		} else {
			res << "Нет блока \"" << name << "\"" << std::endl;
		}
		return res.str();
	}
};


//	ФИ инфошибкаблп
//	выводит сообщения об ошибках класса блока
template<
	typename TF,
	typename BP,
	typename L
>
class ShowClassErrors: public ControlFunc< TF, BP, L >
{
	typedef ControlFunc< TF, BP, L > control_func_t;
	typedef typename control_func_t::block_ptr block_ptr;

public:
	ShowClassErrors( const std::string & name ):
		control_func_t( name )
	{
		control_func_t::add_param( "строка" );	//	Имя блока
	}

	std::string run( block_ptr main_block ){
		std::stringstream res;

		std::string name = control_func_t::get_param( 0 )->to_string();

		auto b = main_block->get_child( name );
		if( b ) {
			auto err = b->get_next_errors();
			if( err.first.size() == 0 ) {
				res << "Ошибок не зарегистрировано" << std::endl;
			} else {
				for( const auto e: err.first ) {
					res << e << std::endl;
				}
				res << "Еще имеется " << err.second << " ошибок" << std::endl;
			}
		} else {
			res << "Нет блока \"" << name << "\"" << std::endl;
		}
		return res.str();
	}
};


}	//	namespace std_fi

}	//	namespace LIBKMS_namespace


#endif
