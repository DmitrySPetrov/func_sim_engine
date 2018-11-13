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

#ifndef __LIBKMS_DETAIL_STD_HPP__
#define __LIBKMS_DETAIL_STD_HPP__

#include <detail/fwd.hpp>

#include <list>
#include <map>
#include <string>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <stack>
#include <vector>
#include <functional>


namespace LIBKMS_namespace {

//!	Для параметров без расширения
struct NoExtension {};


class StdLogger {

public:
	static void notify_priority_change( std::string blockname, double period ) {}

	static void notify_period_change( std::string blockname, double period ) {}

	static void notify_made_periodical( std::string blockname ) {}

	static void notify_made_nonperiodical( std::string blockname ) {}

	static void notify_block_lock( std::string blockname ) {}

	static void notify_block_unlock( std::string blockname ) {}

	static void notify_variable_change( std::string variablename, std::string data ) {}

	static void warning_variable_nonset( std::string variablename ) {}

	static void warning_invalid_variable_type( std::string variablename ) {}

	static void warning_invalid_variable_count( size_t required ) {}

	static void error_invalid_block( std::string name ) {}

	static void error_invalid_type( std::string type ) {}

	static void error_invalid_variable( std::string name ) {}

	static void error_invalid_subprog( std::string id ) {}

	static void error_invalid_control_func( std::string id ) {}
};


template<
	typename Logger
>
class StdScheduler {

public:
	//!	Возвращает список блоков, которые следует вызвать на текущем шаге
	//!	в порядке вызова
	//! \note Порядок вызова - по возрастанию приоритета
	template<
		typename CallablePtr,
		typename BlockContainer,
		typename SubprContainer
	>
	static std::list< CallablePtr > get_nextcall_objs (
			const BlockContainer & bl_map,
			const SubprContainer & func_map,
			double time
		)
	{
		typedef CallablePtr callable_ptr;

		std::multimap< double, callable_ptr >
			clbl_with_priority;

		for( auto & c_p: bl_map ) {
			auto bl = c_p.second;
			if( !bl->is_blocked() &&
				( time > bl->get_lastcall_time_plan() ) )
			{
				//	Пришло время вызова данного блока
				clbl_with_priority.emplace( bl->get_priority(), bl );
			}
		}

		for( const auto & f_p: func_map ) {
			auto fn = f_p.second;
			if( !fn->is_blocked() ) {
				clbl_with_priority.emplace( fn->get_priority(), fn );
			}
		}

		std::list< callable_ptr > res;
		for( const auto & b_p: clbl_with_priority )
			{ res.push_back( b_p.second ); }

		return res;
	}
};


struct StdStringSpec {

static std::pair< std::string, std::string >
	extract_block_name( const std::string & name )
{
	auto dot_it = std::find_if( name.begin(), name.end(), []( char c ) {
			return ( c=='.' );
		} );
	if( dot_it == name.end() ) {
		return std::make_pair( name, "" );
	} else {
		return std::make_pair( std::string( name.begin(), dot_it ),
			std::string( dot_it+1, name.end() ) );
	}
}

static std::string compose_block_name
	( const std::string & parent, const std::string & child )
{
	return parent + "." + child;
}

template< typename ParamPtr >
static std::string print_var_info
	( const std::string & name, const ParamPtr param )
{
	if( !param )
		{ return ""; }

	std::stringstream output;

	output << "\t" << name
		<< "\t" << param->get_type()
		<< "\t" << ( param->is_locked() ? "бл":"" )
		<< "\t" << param->to_string()
		<< "\t" << param->get_desc()
		<< std::endl;

	return output.str();
}

template< typename SubprogPtr >
static std::string print_subp_info
	( const std::string & name, const SubprogPtr subp )
{
	if( !subp )
		{ return ""; }

	std::stringstream output;

	output << "\t" << name
		<< "\t" << subp->get_priority()
		<< "\t" << ( subp->is_blocked() ? "бл":"" )
		<< "\t" << subp->get_power_cond()
		<< std::endl;

	return output.str();
}

};


namespace formula_solver {

//!	Исключение "неизвестная операция"
struct invalid_operation: std::out_of_range {
	explicit invalid_operation( std::string op ):
		std::out_of_range( "Invalid operation" )
	{}
	std::string operation;
};

//!	Исключение "недостаточно аргументов в стеке"
struct invalid_args: std::out_of_range {
	explicit invalid_args( size_t pres, size_t exp ):
		std::out_of_range( "Invalid argument" ),
		args_presented( pres ),
		args_expected( exp )
	{}
	size_t args_presented;
	size_t args_expected;
	std::string op;	//!	Оператор, для которого недостаточно аргументов
	size_t position;	//!	Поцизия оператора в выражении
};

template< typename T >
struct type_default_value {
static T value();
};

template< typename T >
//!	Стандартный класс для взятия значения переменной.
//!	Выполяет приведение типа при помощи метода as()
struct std_value_getter {
template< typename Par >
static T get( Par & p ) {
	return p->template as< T >();
}
};

//!	Шаблон класса - вычислителя значений
template<
	typename T,
	typename Ops,
	template< typename T > class ValueGetter = std_value_getter
>
class StdFormulaSolver {
public:
	typedef formula_solver::invalid_args invalid_args;
	typedef formula_solver::invalid_operation invalid_operation;

private:
	Ops _fops;

public:
	//!	Является ли строка str идентификатором оператора
	bool is_operator( const std::string & str ) const
		{ return _fops.is_operator( str ); }
	//!	Является ли строка str идентификатором операнда
	bool is_operand( const std::string & str ) const
		{ return ( !is_operator( str ) ); }

private:
	std::list< std::string > _s;
	T _default;

public:
	//!	Указываем значение по-умолчанию
	StdFormulaSolver(): _default( type_default_value< T >::value() ) {}
	//!	Извлекаем выражение из строки
	StdFormulaSolver( std::string src, T d=true ): _default( d )
	{
		std::istringstream ss( src );
		_s = std::list< std::string >(
			std::istream_iterator< std::string >( ss ),
			std::istream_iterator< std::string >() );
	}

public:
	//!	Возвращает строку с выражением
	std::string get_cond() const {
		std::string res;
		for( auto o: _s ) {
			res += o + " ";
		}
		return res;
	}
	//!	Возвращает вектор используемых операндов
	std::vector< std::string > get_operands() const {
		std::vector< std::string > res;
		for( auto s: _s ) {
			if( is_operand( s ) ) {
				res.push_back( s );
			}
		}
		return res;
	}
	//!	Извлекает операнды из словаря m и вычисляет выражение
	template< typename Map >
	T solve( const Map & m ) const {
		if( _s.size() == 0 )
			{ return _default; }
		std::stack< T > st;
		for( auto it=_s.begin(); it!=_s.end(); ++it ) {
			if( is_operand( *it ) ) {
				st.push( ValueGetter< T >::get( m.at( *it ) ) );
			} else {
				try {
					_fops.call( *it, st );
				} catch( invalid_args & e ) {
					e.position = std::distance( _s.begin(), it );
					e.op = *it;
					throw e;
				}
			}
		}
		return st.top();
	}
};

//!	Прототип - шаблон класса массива операций
//!	Конструктор должен определить операции
template< typename T >
class Operations {
protected:
	//	Стэк параметров
	typedef std::stack< T > op_stack;

	//	Операции
	typedef std::function< T( op_stack & st ) > stack_func;
	std::map< std::string, stack_func > _ops;

	//	Функции
	typedef std::function< T( T ) > unary_func;
	typedef std::function< T( T, T ) > binary_func;
public:
	//!	Вызвать операцию op над стеком st, вернуть результат
	T call( std::string op, op_stack & st ) const {
		auto it = _ops.find( op );
		if( it == _ops.end() ) {
			throw invalid_operation( op.c_str() );
		} else {
			return it->second( st );
		}
	}
	//!	Является ли строка str идентификатором оператора
	bool is_operator( std::string str ) const {
		return ( _ops.find( str ) != _ops.end() );
	}
protected:
	void add_unary_op( const std::string & op, unary_func f )
	{
		_ops.emplace( op, [ f ]( op_stack & st ){
				if( st.size() < 1 )
					{ throw invalid_args( st.size(), 1 ); }
				T v = st.top(); st.pop();
				st.push( f( v ) );
				return st.top();
			} );
	}
	void add_binary_op( const std::string & op, binary_func f )
	{
		_ops.emplace( op, [ f ]( op_stack & st ){
				if( st.size() < 2 )
					{ throw invalid_args( st.size(), 2 ); }
				T v2 = st.top(); st.pop();
				T v1 = st.top(); st.pop();
				st.push( f( v1, v2 ) );
				return st.top();
			} );
	}
};

//!	Массив операций над логическими переменными
struct LogicOperations: Operations< bool >
{
	LogicOperations() {
		//	Логическое И
		add_binary_op( "&", []( bool a, bool b ) { return a && b; } );
		//	Логическое ИЛИ
		add_binary_op( "|", []( bool a, bool b ) { return a || b; } );
		//	Логическое НЕ
		add_unary_op( "!", []( bool a ) { return !a; } );
	}
};

}	//	namespace formula_solver

namespace formula_solver {
template<>
struct type_default_value< bool > {
static bool value() { return false; }
};
}	//	namespace formula_solver

namespace formula_solver {
//!	Для логического вычислителя относительно произвольных переменных:
//!	при помощи метода is_set() вычисляется логическая интерпретация переменной,
//!	затем вычисляется логическое выражение
//!	\note Используется для вычисления признака работы блока или подпрограммы
template< typename T >
struct is_set_getter {
template< typename Par >
static bool get( Par & p ) {
	return p->is_set();
}
};
}	//	namespace formula_solver

typedef formula_solver::StdFormulaSolver<
		bool,
		formula_solver::LogicOperations,
		formula_solver::is_set_getter
	> StdPowerOnFormulaSolver;


}	//	namespace LIBKMS_namespace

#endif

