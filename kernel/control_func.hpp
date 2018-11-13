#ifndef __LIBKMS_CONTROL_FUNC_HPP__
#define __LIBKMS_CONTROL_FUNC_HPP__


#include <detail/spec.hpp>

#include <string>
#include <vector>
#include <sstream>


namespace LIBKMS_namespace {


//!	Шаблон класса "управляющая функция" (ФИ)
template<
	typename TypeFactory,
	typename BlockPtr,
	typename Logger
>
class ControlFunc {

public:
	ControlFunc( std::string name ) {}
	virtual ~ControlFunc() {}

private:
	typedef TypeFactory type_factory_t;
	typedef typename type_factory_t::parameter_ptr parameter_ptr;

	std::vector< parameter_ptr > _params;

protected:
	//!	Добавить параметр
	//!	\return номер добавленного параметра
	size_t add_param( std::string type ) {
		auto p_var = type_factory_t::instance().create( type );
		if( !p_var ) {
			Logger::error_invalid_type( type );
			return -1;
		}
		_params.push_back( p_var );
		return ( _params.size() - 1 );
	}

	//!	Вернуть указатель на параметр
	parameter_ptr get_param( size_t num ) {
		if( num >= _params.size() ) {
			return parameter_ptr();
		}
		return _params[ num ];
	}

public:
	//!	Установить значение параметров
	//!	\return признак успешного завершения операции
	bool set_parameter_values( std::vector< std::string > values ) {
		if( values.size() != _params.size() ) {
			Logger::warning_invalid_variable_count( _params.size() );
			return false;
		}
		bool bResult = true;
		for( size_t i=0; i<_params.size(); i++ ) {
			if( !_params[i]->from_string( values[i] ) ) {
				std::stringstream num;
				num << i;
				Logger::warning_invalid_variable_type( num.str() );
				bResult = false;
			}
		}
		return bResult;
	}

	//!	Вернуть количество параметров ФИ
	size_t get_parameter_count( ) {
		return _params.size();
	}

public:
	typedef BlockPtr block_ptr;

	//!	Выполнить действие, предусмотренное функцией
	//!	\return текстовое сообщение, выводимое командой
	virtual std::string run( block_ptr main_block )=0;

};


template<
	typename TypeFactory,
	typename BlockPtr,
	typename Logger,
	template< typename U > class Spec
>
class ControlFuncStorage {

public:
	typedef ControlFuncStorage< TypeFactory, BlockPtr, Logger, Spec >
		control_func_storage_t;
	typedef ControlFunc< TypeFactory, BlockPtr, Logger >
		control_func_t;

public:
	//!	Возвращаем ссылку на синглтон
	static control_func_storage_t & instance() noexcept {
		static control_func_storage_t instance;
		return instance;
	}

private:
	ControlFuncStorage() {
		add_func< std_fi::GetValue >( "инфп" );
		add_func< std_fi::SetValue >( "устп" );
		add_func< std_fi::GetVarList >( "инфпблп" );
		add_func< std_fi::LockParam >( "блп" );
		add_func< std_fi::LockSubprog >( "блпп" );
		add_func< std_fi::GetSubpList >( "инфппблп" );
		add_func< std_fi::LockBlock >( "блблп" );
		add_func< std_fi::ShowClassErrors >( "инфошибкаблп" );
	}

	~ControlFuncStorage() {};

	ControlFuncStorage( const ControlFuncStorage & );
	ControlFuncStorage operator=( const ControlFuncStorage & );

private:
	typedef typename Spec< control_func_t >::ptr control_func_ptr;
	typedef typename Spec< control_func_t >::map control_func_map;
	control_func_map _func;

public:
	//! Добавляем новую функцию
	template<
		template<
			typename T,
			typename U,
			typename Y
		> class F
	>
	bool add_func( std::string name ) {
		typedef F< TypeFactory, BlockPtr, Logger > func_t;
		typename Spec< func_t >::ptr f( new func_t( name ) );
		if( !_func.emplace( name, std::move( f ) ).second ) {
			Logger::error_invalid_control_func( name );
			return false;
		}
		return true;
	}

	//! Получаем указатель на функцию по имени
	const control_func_ptr get( const std::string & name ) const {
		auto f = _func.find( name );
		if( f == _func.end() ) {
			Logger::error_invalid_control_func( name );
			return shared_ptr_spec< control_func_t >::null_ptr();
		}
		return f->second;
	}

};


}	//	namespace LIBKMS_namespace


#endif
