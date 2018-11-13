#ifndef __LIBKMS_BLOCK_CLASS_HPP__
#define __LIBKMS_BLOCK_CLASS_HPP__

#include <block_class/names.hpp>

#include <limits>
#include <string>
#include <sstream>

#include <block_class/udp_exchange.hpp>


namespace LIBKMS_namespace {


//!	Прототип описания класса блока
/*!
Класс блока определяет подпрограмму (subprogram_t), вызываемую вместе с блоком.

Подпрограмма должна добавляться в хранилище подпрограмм автоматически при
считывании конфигурации с именем, определенном в классе-наследнике.
Подпрограмма возвращает словарь переменных, которые могут использоваться или не
использоваться в блоке.
Для добавления действующей функции подпрограммы в хранилище использовать метод
add_func(...).
*/
template<
	typename FunctionStorage,
	typename Subprogram,
	typename SubprogramPtr,
	typename BlockPtr,
	template< typename T > class BlClSpec
>
class BlockClass {

public:
	typedef BlockClass
			< FunctionStorage, Subprogram, SubprogramPtr, BlockPtr, BlClSpec >
		block_class_t;
	typedef typename BlClSpec< block_class_t >::ptr ptr_t;

	typedef typename FunctionStorage::parameter_map parameter_map;

	BlockClass( const std::string & subpr_name ):
		_has_err( false ),
		_subprog_name( subpr_name )
	{}

	virtual ~BlockClass() {};

public:
	//!	Возвращает имя подпрограммы, определенной для класса блока
	std::string get_subprog_name()
		{ return _subprog_name; }

protected:
	//!	Возвращает объект-подпрограмму для блока
	virtual SubprogramPtr get_subprog( const std::string & ) = 0;

protected:
	//!	Сформировать и вернуть указатель на подпрограмму
	//! \note Для одного и того же класса блока в зависимости от конфигурации
	//!		могут быть определены различные функции, поэтому требуется суффикс
	//!		к имени функции add.
	//!	\param add суффикс к имени функции
	//!	\param f функция, которая будет положена в хранилище и использована в
	//!		подпрограмме
	SubprogramPtr construct_subprog(
		const typename FunctionStorage::function_t & f,
		const std::string & suffix = "",
		double priority = std::numeric_limits< double >::max() ) noexcept
	{
		std::string func_name = _subprog_name;
		if( suffix.length() > 0 )
			{ func_name += "_" + suffix; }

		if( !FunctionStorage::instance().add_func( func_name, f ) ) {
			std::ostringstream ss;
			ss << "Ошибка при добавлении функции с именем \"" << _subprog_name
				<< "\". Вероятно, такая функция уже есть";
			add_error_msg( ss.str() );
			return SubprogramPtr();
		}

		SubprogramPtr subprog(
			new Subprogram( _subprog_name, func_name, 
				priority ) );

		return subprog;
	}

public:
	//!	Проверить, все ли необходимые данные есть в блоке
	virtual bool join_with_block( const BlockPtr & bl,
			const std::string & suffix="" )
	{
		SubprogramPtr sub;
		if( suffix.length() > 0 ) {
			sub = get_subprog( suffix );
		} else {
			const void * ptr = static_cast< const void * >( &( *bl ) );
			std::ostringstream ss; ss << ptr;
			sub = get_subprog( ss.str() );
		}
		if( !sub ) {
			//	Почему-то не удалось создать подпрограмму
			add_error_msg( "Ошибка при создании подпрограммы" );
			return false;
		}
		if( !bl->add_subprog( _subprog_name, sub ) ) {
			add_error_msg( "Не удалось добавить подпрограмму в блок" );
			return false;
		}
		return true;
	}

	//!	Вернуть список сообщений об ошибках
	std::string get_error_msg() {
		std::string res = _err_msg; _err_msg.clear();
		_has_err = false;
		return res;
	}

	bool has_errors() { return _has_err; }

protected:
	//!	Установить сообщение об ошибке
	void set_error_msg( const std::string & s ) {
		_err_msg = s;
		_has_err = true;
	}

	//!	Добавить сообщение об ошибке
	void add_error_msg( const std::string & s ) {
		if( _err_msg.length() > 0 ) {
			_err_msg = s + "\n" + _err_msg;
		} else {
			_err_msg = s;
		}
		_has_err = true;
	}

private:
	std::string _err_msg;
	bool _has_err;

public:
	virtual ptr_t clone() const=0;

private:
	std::string _subprog_name;

protected:
	//!	Проверить, есть ли переменная в блоке и добавить ее в список своих
	bool validate_n_add_var( const BlockPtr & bl, const std::string & name ) {
		auto var_obj = bl->get_variable( name );
		if( !var_obj ) {
			std::ostringstream res;
			res << "Переменная \"" << name << "\" отсутствует в блоке";
			this->add_error_msg( res.str() );
			return false;
		}
		this->_vars.emplace( name, var_obj->clone() );
		return true;
	}

protected:
	parameter_map _vars;

};	//	class BlockClass


template<
	typename FunctionStorage,
	typename Subprogram,
	typename SubprogramPtr,
	typename BlockPtr,
	typename ConfigNode,
	template< typename U > class BlClSpec
>
struct BlockClassWithConfig:
	BlockClass< FunctionStorage, Subprogram, SubprogramPtr, BlockPtr, BlClSpec >
{
	typedef BlockClass
			< FunctionStorage, Subprogram, SubprogramPtr, BlockPtr, BlClSpec >
		block_class_t;

	BlockClassWithConfig( const std::string & subpr_name ):
		block_class_t( subpr_name )
	{}

	virtual bool read_class_config( const ConfigNode & node )=0;

	virtual SubprogramPtr get_subprog( const std::string & ) = 0;

};	//	class BlockClassWithConfig


//!	Фабрика классов блоков
template<
	typename FunctionStorage,
	typename Subprogram,
	typename SubprogramPtr,
	typename BlockPtr,
	template< typename U > class BlClSpec
>
class BlockClassFactory {
public:
	typedef BlockClassFactory<
			FunctionStorage,
			Subprogram,
			SubprogramPtr,
			BlockPtr,
			BlClSpec	
		> block_class_factory_t;

	typedef BlockClass<
			FunctionStorage,
			Subprogram,
			SubprogramPtr,
			BlockPtr,
			BlClSpec
		> block_class_t;

	template< typename T >
	struct block_class_with_config {
		typedef BlockClassWithConfig<
				FunctionStorage,
				Subprogram,
				SubprogramPtr,
				BlockPtr,
				T,
				BlClSpec
			> type;
		typedef typename BlClSpec< type >::ptr ptr_t;
	};	//	struct block_class_with_config

	typedef typename BlClSpec< block_class_t >::ptr block_class_ptr;
	typedef typename BlClSpec< block_class_t >::map block_class_map;

public:
	//!	Вернуть экземпляр фабрики классов блоков
	static block_class_factory_t & instance() noexcept {
		static block_class_factory_t instance;
		return instance;
	}

	//!	Создать экземпляр класса блока
	block_class_ptr create( const std::string & classname ) const noexcept
	{
		auto it = _blcl.find( classname );
		if( it == _blcl.end() ) {
			return block_class_ptr();
		}
		return it->second->clone();
	}

	template< typename T >
	static typename block_class_with_config< T >::ptr_t
		with_config( const block_class_ptr p )
	{
		return BlClSpec< block_class_t >::template dynamic_ptr_cast
			< typename block_class_with_config< T >::type >( p );
	}

	//!	Добавить новый класс блоков
	//!	\return добавилось ли
	template< typename T >
	bool add_block_class( const std::string & classname )
	{
		//	Костыль: если объединить в одну строчку - оптимизатор съедает
		//	добавление
		auto blcl = std::make_pair
			( classname, typename BlClSpec< T >::ptr( new T ) );
		return _blcl.emplace( blcl ).second;
	}

private:
	block_class_map _blcl;

private:

	BlockClassFactory() {}

	BlockClassFactory( const block_class_factory_t & );
	BlockClassFactory & operator=( const block_class_factory_t & );
	
};	//	class BlockClassFactory


}	//	namespace LIBKMS_namespace


#endif

