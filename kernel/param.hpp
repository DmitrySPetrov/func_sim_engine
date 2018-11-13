#ifndef __LIBKMS_PARAM_HPP__
#define __LIBKMS_PARAM_HPP__


#include <param/convert.hpp>
#include <detail/spec.hpp>
#include <detail/std.hpp>

#include <algorithm>
#include <string>
#include <cstring>


namespace LIBKMS_namespace {

namespace types {

//!	Для осуществления возможности возврата holder< T > по T
template< typename T, typename P >
struct get_return_type {
	typedef T ogiginal_t;
	typedef T return_t;
};

}	//	namespace types


//!	Класс, от которого должны наследоваться все классы параметров
//	TODO: Описать нулевой оператор копирования и нулевой метод
//		сравнения
template<
	class Ext,			//	Класс-расширение
	template < typename U > class Spec >
class Parameter: public Ext
{
	std::string _type;
	std::string _desc;
	bool _locked;

public:

	typedef Parameter< Ext, Spec > parameter_t;
	typedef typename Spec< parameter_t >::ptr ptr_t;

	static ptr_t null_ptr() { return Spec< parameter_t >::null_ptr(); }

public:

	Parameter():
		_locked( false )
	{}

	Parameter( const std::string type_name ):
		_type( type_name ),
		_locked( false )
	{}

	virtual ~Parameter() { }

	virtual ptr_t clone()=0;

	//!	Вернуть представление данных в виде строки
	virtual std::string to_string() const=0;

	//!	Записать значение из строки
	virtual bool from_string( const std::string & )=0;

	//!	Вернуть представление данных в виде вектора байтов
	virtual std::vector< uint8_t > to_bytes() const=0;

	//! Записать значение из набора байтов
	virtual bool from_bytes( const uint8_t *, size_t )=0;

	//!	Сравнить значения
	virtual bool is_equal( const parameter_t & another ) const=0;

	//!	Является ли параметр установленным (для power-переменных)
	virtual bool is_set() const=0;

	//!	Вернуть имя типа
	std::string get_type() const
		{ return _type; }

	//!	Скопировать значение из указанного контейнера
	//!	\return Изменилось ли значение
	virtual bool copy_data( parameter_t & )=0;

	//!	Попытка преобразования к нужному типу
	//!	\return нуль, если попытка не удалась
	template< typename T >
	typename types::get_return_type< T, parameter_t >::return_t as() {
		return obj_as<
				typename types::get_return_type< T, parameter_t >::return_t,
				parameter_t
			>::as( *this );
	}
	template< typename T >
	const typename types::get_return_type< T, parameter_t >::return_t
		c_as() const
	{
		return obj_as<
				typename types::get_return_type< T, parameter_t >::return_t,
				parameter_t
			>::c_as( *this );
	}

	//!	Устанавливает описание переменной
	void set_desc( const std::string & d )
		{ _desc = d; }

	//!	Возвращает описание переменной
	const std::string & get_desc( ) const
		{ return _desc; }

	//!	Возвращает признак блокировки 
	bool is_locked( ) const
		{ return _locked; }

	//!	Блокирует переменную
	void set_lock( bool is_locked )
		{ _locked = is_locked; }

};	// struct parameter


namespace util {

template< typename E, template< typename U > class S >
inline std::string to_string( const Parameter< E, S > & src ) {
	return src.to_string();
}

template< typename E, template< typename U > class S >
inline bool from_string( const std::string & s, Parameter< E, S > & dst ) {
	return dst.from_string( s );
}

template< typename E, template< typename U > class S >
inline bool from_bytes( const uint8_t * b, size_t s, Parameter< E, S > & dst ) {
	return dst.from_bytes( b, s );
}

template< typename Container, typename E, template< typename U > class S >
inline bool from_bytes( const Container & src, Parameter< E, S > & dst ) {
	return dst.from_bytes( src.data(), src.size() );
}

template< typename E, template< typename U > class S >
inline size_t to_bytes( const Parameter< E, S > & src, uint8_t * dst, size_t len ) {
	auto res = src.to_bytes();
	size_t len_ = std::min( len, res.size() );
	std::memcpy( dst, res.data(), len_ );
	return len_;
}

template< typename Container, typename E, template< typename U > class S >
inline size_t to_bytes( const Parameter< E, S > & src, Container & dst ) {
	auto res = src.to_bytes();
	std::copy( res.begin(), res.end(), std::back_inserter( dst ) );
	return dst.size();
}

template< typename E, template< typename U > class S >
inline bool is_equal( const Parameter< E, S > & one, const Parameter< E, S > & another )
{
	return one.is_equal( another );
}

template< typename E, template< typename U > class S >
inline bool copy_data( Parameter< E, S > & src, Parameter< E, S > & dst )
{
	return dst.copy_data( src );
}

}	//	namespace util


}	//	namespace LIBKMS_namespace


#endif

