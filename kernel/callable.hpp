#ifndef __LIBKMS_CALLABLE_HPP__
#define __LIBKMS_CALLABLE_HPP__


#include <string>
#include <list>
#include <vector>

#include <boost/date_time/posix_time/posix_time.hpp>
namespace pt = boost::posix_time;


namespace LIBKMS_namespace {


template<
	typename Logger,
	typename ParameterMap,
	typename FormulaSolver
>
class Callable {

public:
	typedef ParameterMap parameter_map;

private:
	//	Пустой список параметров
	static const parameter_map no_params;

protected:
	std::string _name;	//	Имя объекта

private:
	pt::ptime	_start;	//	Время создания объекта
	double	_lastcall_time;	//	Время последнего вызова в секундах от запуска моделирования
	double	_process_time;	//	Время исполнения в секундах
	double	_delta_time;	//	Время от последнего вызова

protected:
	double	_priority;	//	Приоритет

	bool	_isblocked;	//	Является ли заблокированным

	FormulaSolver _cond_s;	//	Объект решателя формул

public:
	Callable( std::string name, double priority, bool isblocked ):
		_name( name ),
		_start( pt::microsec_clock::universal_time() ),
		_lastcall_time( 0.0 ), _process_time( 0.0 ),
		_delta_time( 0.0 ),
		_priority( priority ), _isblocked( isblocked ),
		_cond_s( "", true )
	{}

	void set_name( std::string name ) {
		_name = name;
	}

	std::string get_name( ) {
		return _name;
	}

	virtual ~Callable() {}

public:
	//!	Осуществить вызов объекта
	//! \param time текущее время
	//!	\param block_vars переменные родительского блока
	//!	\param upper_vars переменные родителя родительского блока
	//!	\return новые значения для переменных родительского блока
	virtual parameter_map call(
			double time,
			const parameter_map & block_vars,
			const parameter_map & upper_vars
		)=0;

private:
	void pre_call() {
		double now = ( pt::microsec_clock::universal_time() - _start )
			.total_microseconds() * 1e-6;
		_delta_time = now - _lastcall_time;
		_lastcall_time = now;
	}
	void post_call() {
		double now = ( pt::microsec_clock::universal_time() - _start )
				.total_microseconds() * 1e-6;
		_process_time = now - _lastcall_time;
	}

public:
	parameter_map operator()(
			double time,
			const parameter_map & block_vars=no_params,
			const parameter_map & upper_vars=no_params
		)
	{
		pre_call();
		auto res = call( time, block_vars, upper_vars );
		post_call();
		return res;
	}

	//!	Запомнить текущее время как время последнего вызова
	void touch() {
		pre_call();
	}

public:
	//! Установить значение приоритета
	void set_priority( double priority ) noexcept
	{
		if( _priority != priority ) {
			_priority = priority;
			Logger::notify_priority_change( _name, priority );
		}
	}

	//! Установить блокировку блока
	void set_lock( bool lock ) noexcept
	{
		if( _isblocked != lock ) {
			_isblocked = lock;
			if( lock ) {
				Logger::notify_block_lock( _name );
			} else {
				Logger::notify_block_unlock( _name );
			}
		}
	}

	double get_lastcall_time() const noexcept
		{ return _lastcall_time; }
	double get_process_time() const noexcept
		{ return _process_time; }
	double get_delta_time() const noexcept
		{ return _delta_time; }

	double get_priority() const noexcept
		{ return _priority; }
	bool is_blocked() const noexcept
		{ return _isblocked; }

	//!	Является ли объект включенным при заданных входных переменных
	bool is_powered( const parameter_map & v ) const {
		return _cond_s.solve( v );
	}
	//!	Считать формулу для определения включения объекта
	bool set_power_cond( const std::string & src ) {
		_cond_s = FormulaSolver( src );
		return true;
	}

	//!	Вернуть строку с формулой
	std::string get_power_cond() const {
		return _cond_s.get_cond();
	}

	//!	Вернуть список переменных
	std::vector< std::string > get_power_vars() const {
		return _cond_s.get_operands();
	}
};	//	class Callable


template< typename Logger, typename ParameterMap, typename FormulaSolver >
const ParameterMap Callable< Logger, ParameterMap, FormulaSolver >::no_params;


}	//	namespace LIBKMS_namespace


#endif
