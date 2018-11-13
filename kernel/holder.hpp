#ifndef __LIBKMS_HOLDER_HPP__
#define __LIBKMS_HOLDER_HPP__


#include <param.hpp>
#include <param/convert.hpp>
#include <detail/fwd.hpp>
#include <detail/std_types.hpp>

#include <sstream>
#include <string>
#include <iostream>
#include <cstring>


namespace LIBKMS_namespace {


namespace holder_util {


//!	Шаблон функции для печати данных в строку
template< typename T >
inline std::string to_string( const T & m );

//!	Шаблон функции для извлечения данных из строки
template< typename T >
inline bool from_string( const std::string & src, T & dst ) {
	std::istringstream iss( src );
	T d; iss >> d;
	if( iss.eof() && !iss.fail() ) {
		dst = d;
		return true;
	} else {
		return false;
	}
}

//!	Шаблон функции для представления данных в виде вектора байтов
template< typename T >
inline std::vector< uint8_t > to_bytes( const T & src ) {
	auto buf = reinterpret_cast< const uint8_t * >( & src );
	return std::vector< uint8_t >( buf,
		buf + sizeof( T ) / sizeof( uint8_t ) );
}

//!	Шаблон функции для извлечения данных из набора байтов
template< typename T >
inline bool from_bytes( const uint8_t * src, size_t len, T & dst ) {
	if( len != sizeof( T ) )
		{ return false; }
	std::memcpy( & dst, src, sizeof( T ) );
	return true;
}

//!	Шаблон функции для проверки установки значения
template< typename T >
inline bool is_set( const T & src ) {
	return src;
}

//!	Шаблон для копирования данных
template< typename T >
inline bool copy_data( T & dst, T & src ) {
	if( dst == src ) {
		return false;
	}
	dst = src;
	return true;
}


//=========================================================
//	Специализации шаблонов
template<>
inline std::string to_string( const std_types::tBool & d )
	{ return std::to_string( d ); }

template<>
inline std::string to_string( const std_types::tReal & d )
	{ return std::to_string( d ); }

template<>
inline std::string to_string( const std_types::tInt & d )
	{ return std::to_string( d ); }

template<>
inline std::string to_string( const std_types::tString & value )
	{ return value; }

template<>
inline bool from_string( const std::string & src, std_types::tString & dst ) {
	dst = src;
	return true;
}

template<>
inline bool from_bytes( const uint8_t * src, size_t len, std_types::tBool & dst )
{
	if( len != 1 && len != 2 )
		{ return false; }
	std::memcpy( & dst, src, sizeof( std_types::tBool ) );
	return true;
}

template<>
inline bool from_bytes( const uint8_t * src, size_t len, std_types::tString & dst )
{
	dst = std::string( reinterpret_cast< const char * >( src ), len );
	return true;
}

template<>
inline std::vector< uint8_t > to_bytes( const std_types::tBool & src ) {
	std::vector< uint8_t > res( 2, 0 ); res[0] = src;
	return res;
}

template<>
inline std::vector< uint8_t > to_bytes( const std_types::tInt & src ) {
	const uint8_t * p_src = reinterpret_cast< const uint8_t * >( &src );
	return std::vector< uint8_t >( p_src, p_src + sizeof( std_types::tInt ) );
}

template<>
inline std::vector< uint8_t > to_bytes( const std_types::tReal & src ) {
	const uint8_t * p_src = reinterpret_cast< const uint8_t * >( &src );
	return std::vector< uint8_t >( p_src, p_src + sizeof( std_types::tReal ) );
}

template<>
inline std::vector< uint8_t > to_bytes( const std_types::tString & src ) {
	return std::vector< uint8_t >( src.begin(), src.end() );
}


template<>
inline bool is_set( const std_types::tString & src ) {
	return src.length() > 0;
}


}	//	namespace holder_util


//!	Специализация для преобразования parameter в Holder< T >
template< typename T, typename P >
struct obj_as< Holder< T, P >, P > {

typedef Holder< T, P > holder_t;

static holder_t & as( P & obj ) {
	return dynamic_cast< holder_t & >( obj );
}

static const holder_t & c_as( const P & obj ) {
	return dynamic_cast< const holder_t & >( obj );
}

};	//	template< typename T > struct obj_as< Holder< T > >


//!	Класс-контейнер для стандартных типов
template<
	typename T,
	typename Parameter
>
class Holder: public Parameter
{
	typedef Parameter parameter_t;
	typedef typename parameter_t::ptr_t parameter_ptr;
	typedef Holder< T, Parameter > holder_t;

protected:
	T _data;

public:
	Holder():
		parameter_t( types::type_name< T >() ),
		_data( types::default_value< T >() )
	{}

	Holder( const T & data ):
		parameter_t( types::type_name< T >() ),
		_data( data )
	{}

public:
	//!	Вернуть сохраненное значение
	operator T() const { return _data; }

	//!	Записать новое значение
	holder_t & operator=( const T val ) { _data = val; return *this; }

	void set_value( const T val ) { _data = val; }
	T get_value() const { return _data; }

	parameter_ptr clone() {
		holder_t * res = new holder_t();
		res->copy_data( *this );
		return parameter_ptr( res );
	}

	//!	Вернуть представление данных в виде строки
	std::string to_string() const
		{ return holder_util::to_string( _data ); }

	//!	Записать значение из строки
	bool from_string( const std::string & src ) {
		T old = _data;
		if( holder_util::from_string( src, _data ) ) {
			return true;
		} else {
			_data = old;
			return false;
		}
	}

	//!	Вернуть представление данных в виде вектора байтов
	std::vector< uint8_t > to_bytes() const
		{ return holder_util::to_bytes( _data ); }

	//!	Записать значение из набора байтов
	bool from_bytes( const uint8_t * b, size_t s ) {
		T old = _data;
		if( holder_util::from_bytes( b, s, _data ) ) {
			return true;
		} else {
			_data = old;
			return false;
		}
	}

	//!	Проверка равны ли значения
	bool is_equal( const parameter_t & another ) const {
		auto p_a = dynamic_cast< const holder_t * >( &another );
		if( !p_a )
			{ return false; }
		return _data == p_a->_data;
	}

	//!	Является ли параметр установленным (для power-переменных)
	bool is_set() const {
		return holder_util::is_set( _data );
	}

	//!	Скопировать значение из указанного контейнера
	//	TODO: функция с двумя параметрами - виртуальными классами
	bool copy_data( parameter_t & a ) {
		auto & h_a = dynamic_cast< holder_t & >( a );
		return holder_util::copy_data( _data, h_a._data );
	}

};	//	template< typename T > class Holder


}	//	namespace LIBKMS_namespace

namespace LIBKMS_namespace {

namespace types {

template< typename P >
struct get_return_type< std_types::tBool, P > {
	typedef Holder< std_types::tBool, P > return_t;
};

template< typename P >
struct get_return_type< std_types::tInt, P > {
	typedef Holder< std_types::tInt, P > return_t;
};

template< typename P >
struct get_return_type< std_types::tReal, P > {
	typedef Holder< std_types::tReal, P > return_t;
};

template< typename P >
struct get_return_type< std_types::tString, P > {
	typedef Holder< std_types::tString, P > return_t;
};

}	//	namespace LIBKMS_namespace::types


namespace exception {

class invalid_var_name: public std::out_of_range {
public:
	invalid_var_name( const std::string & name ):
		std::out_of_range( "Inexistent parameter name" ),
		_name( name )
	{}

	std::string get_name() {
		return _name;
	}

private:
	std::string _name;
};

class invalid_type: public std::bad_cast {
public:
	invalid_type( const std::string & name ):
		std::bad_cast(),
		_name( name )
	{}

	std::string get_name() {
		return _name;
	}

private:
	std::string _name;
};

} //	namespace exception

namespace map_tools {

//!	Извлечь параметр определенного типа из шаблона
template< typename T >
struct get {
	template< typename Pmap >
	T operator ()( const Pmap & map, const std::string & name ) {
		auto it = map.find( name );
		if( it == map.end() ) {
			throw exception::invalid_var_name( name );
		}
		auto ptr = it->second->clone();
		try {
			T res( ptr->template as< T >() );
			return res;
		} catch( std::bad_cast & e ) {
			throw exception::invalid_type( name );
		}
	}
};


//!	Добавить параметр определенного типа в массив
template< typename T, typename Pmap >
void add_parameter_to_map( Pmap & map, const std::string & name, T val )
{
	map[ name ] = typename Pmap::mapped_type
		( new Holder< T, typename Pmap::mapped_type::element_type >( val ) );
}

}	//	namespace map_tools

}	//	namespace LIBKMS_namespace


#endif
