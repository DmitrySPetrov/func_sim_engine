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
