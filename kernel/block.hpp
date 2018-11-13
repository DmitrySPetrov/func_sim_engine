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

#ifndef __LIBKMS_BLOCK_HPP__
#define __LIBKMS_BLOCK_HPP__


#include <block_class.hpp>
#include <callable.hpp>
#include <detail/spec.hpp>
#include <detail/fwd.hpp>
#include <detail/std.hpp>


#include <string>
#include <map>
#include <set>


namespace LIBKMS_namespace {


//!	Шаблонный класс Block, отображающий блок процесс.
/*!>
Блок процесс (БЛП) является основной единицей моделирования.
БЛП верхнего уровня является всей моделью.
БЛП верхнего уровня периодически вызывается внешней подпрограммой.

БЛП может содержать в себе БЛП низшего уровня.
При каждом вызове БЛП высшего уровня последовательно вызывает БЛП низшего
уровня в зависимости от их приоритета и периода вызова.
За порядок вызова БЛП отвечает планировщик (Scheduler).
Поскольку вызов БЛП низшего уровня происходит во время работы БЛП высшего
уровня, период вызова последнего не может быть меньше чем у родительского БЛП.

Для передачи параметров между БЛП служат переменные.
С помощью переменных можно обеспечить обмен только между блоками одного уровня.
Каждый блок имеет набор собственных (SET) и используемых (USE) переменных.
SET и USE переменные должны иметь символические имена, не содержащие точек и
пробелов.

При вызове БЛП низшего уровня из БЛП высшего уровня производится копирование USE
переменных БЛП низшего уровня из переменных БЛП высшего уровня.
По окночнании работы БЛП низшего уровня производится копирование SET переменных
из БЛП высшего уровня.
Копируются только те переменные, которые указаны во внешней конфигурации - базе
данных (БД).
Если переменная с именем, указанным в БД, отсутствует в блоке, она не
копируется.
При этом должно формироваться однократное сообщение об ошибке.

Вызов БЛП, запись обновленных значений переменных и возникающие при этом ошибки
фиксируются в протоколе при помощи компонента Logger.

Данный шаблонный класс является прототипом для БЛП, входящих в состав модели.
Последние должны содержать следующую дополнительную информацию:
<ul>
<li></li>
</ul>

Как БЛП совершает полезную работу, т.е. обрабатывает USE переменные?

Создание БЛП:
<ul>
<li>Объявить класс, наследующий от Block.</li>
<li>В конструкторе </li>
</ul>

Порядок работы с БЛП:
<ul>
<li>...</li>
<li></li>
<li></li>
<li></li>
<li></li>
<li></li>
</ul>
*/

template<
	typename TypeFactory,
	template< typename U > class ParamSpec,
	typename Scheduler,
	typename Logger,
	template< typename U > class SubprSpec,
	typename StringSpec,
	template< typename U > class BlClSpec,
	typename FormulaSolver
>
class Block: public Callable<
		Logger,
		typename ParamSpec< typename TypeFactory::parameter_t >::map,
		FormulaSolver
	>
{
public:
	typedef Block<
			TypeFactory,
			ParamSpec,
			Scheduler,
			Logger,
			SubprSpec,
			StringSpec,
			BlClSpec,
			FormulaSolver
		> block_t;

	typedef TypeFactory type_factory_t;
	typedef typename type_factory_t::parameter_t parameter_t;
	typedef typename parameter_t::ptr_t parameter_ptr;
	typedef typename ParamSpec< parameter_t >::map parameter_map;

	typedef Callable< Logger, parameter_map, FormulaSolver > callable_t;

	typedef Subprogram< Logger, parameter_map, FormulaSolver > subprogram_t;
	typedef typename SubprSpec< subprogram_t >::ptr subprogram_ptr;
	typedef typename SubprSpec< subprogram_t >::map subprogram_map;

	typedef FunctionStorage< parameter_map > function_storage_t;

	typedef typename SubprSpec< block_t >::ptr ptr_t;
	typedef typename SubprSpec< block_t >::map map_t;

	typedef BlockClassFactory<
			function_storage_t,
			subprogram_t,
			subprogram_ptr,
			ptr_t,
			BlClSpec
		> block_class_factory_t;
	typedef typename block_class_factory_t::block_class_ptr block_class_ptr;

	typedef typename subprogram_t::user_exception subp_user_exception;

protected:
	std::string _classname;	//	Имя класса блока

public:

	//!	Конструктор класса
	Block( const std::string classname, const std::string name ):
		callable_t( name, 1.0, false ),
		_classname( classname ),
		_is_blocked_in_stop( true ),
		_isperiodical( false ), _period( 1.0 ),
		_lastcall_plan( 0.0 ), _power_prev_step( false )
	{
		_class = block_class_factory_t::instance().create( classname );
	}

	virtual ~Block() {}


private:
//	Свойства вызова блока
//	Используется при работе планировщика

	bool	_is_blocked_in_stop;	//	выполняется ли в режиме СТОП

	bool	_isperiodical;	//	Периодический ли блок
	double	_period;	//	Период вызова
	double	_lastcall_plan;	//	Время последнего вызова

	block_class_ptr _class;	//	Данные класса блока


public:
	//!	Возвращает время последнего вызова по плану
	double get_lastcall_time_plan() { return _lastcall_plan; }

private:
	void block_pre_call( double time ) {
		//	time - текущее время
		_lastcall_plan += _period;
	}


public:
//	Функции, меняющие свойства вызова блока

	//!	Установить период вызова
	void set_period( double period ) noexcept
	{
		if( _period != period ) {
			_period = period;
			Logger::notify_period_change( callable_t::_name, period );
		}
	}

	//!	Сделать блок периодическим
	void make_periodical() noexcept
	{
		if( !_isperiodical ) {
			_isperiodical = true;
			Logger::notify_made_periodical( callable_t::_name );
		}
	}

	//!	Сделать блок вызываемым по обновлению
	void make_nonperiodical() noexcept
	{
		if( _isperiodical ) {
			_isperiodical = false;
			Logger::notify_made_nonperiodical( callable_t::_name );
		}
	}


	bool is_periodical() const noexcept
		{ return _isperiodical; }
	double get_period() const noexcept
		{ return _period; }

	//!	Установить признак блокировки в СТОПе
	void set_blocked_in_stop( bool bl ) noexcept
	{
		_is_blocked_in_stop = bl;
	}

	//!	Проверить, является ли заблокированным в СТОПе
	bool is_blocked_in_stop() noexcept
	{
		return _is_blocked_in_stop;
	}

public:
	block_class_ptr get_block_class( ) {
		return _class;
	}


private:
	//	Переменные агрегированных блоков
	parameter_map _vars;

public:
	//!	Добавить переменную
	bool add_variable( const std::string & name, const std::string & type ) noexcept
	{
		parameter_ptr p = type_factory_t::instance().create( type );
		if( p == 0 ) {
			//	Такого типа нет
			Logger::error_invalid_type( type );
			return false;
		}
		return add_variable( name, p );
	}

	//!	Добавить уже созданную переменную
	bool add_variable( const std::string & name, parameter_ptr & p ) noexcept
	{
		auto _i = _vars.emplace( name, std::move( p ) );
		if( !_i.second ) {
			//	Переменная с таким именем уже есть
			Logger::error_invalid_variable( name );
		}
		return _i.second;
	}

	//!	Получить указатель на переменную с именем
	parameter_ptr get_variable( const std::string & name ) const noexcept
	{
		auto r = StringSpec::extract_block_name( name );
		if( r.second.length() ) {
			auto child_p = _children.find( r.first );
			if( child_p == _children.end() ) {
				Logger::error_invalid_block( r.first );
				return parameter_ptr();
			} else {
				return child_p->second->get_variable( r.second );
			}
		} else {
			//	Не найдено, значит ищем переменную
			auto _it = _vars.find( name );
			if( _it == _vars.end() ) {
				//	Такой переменной нет
				Logger::error_invalid_variable( name );
				return parameter_ptr();
			}
			return _it->second;
		}
	}

	//!	Записать значения разрешенных для записи переменных
	//!	\return список записанных переменных
	std::set< std::string > write_variables
		( const std::set< std::string > & allowed, parameter_map & val )
	{
		std::set< std::string > res;

		for(auto & v_p: val ) {
			auto dst = _vars.find( v_p.first );
			if( dst == _vars.end() ) {
				Logger::error_invalid_variable( v_p.first );
				continue;
			}
			parameter_ptr v = dst->second;

			//	Если переменная заблокирована - игнорируем
			if( v->is_locked() )
				{ continue; }

			//	Копируем значение
			if( v->copy_data( *( v_p.second ) ) ) {
				//	Если значение изменилось - сообщаем об изменении
				Logger::notify_variable_change( v_p.first, v_p.second->to_string() );
			}

			res.insert( v_p.first );
		}

		return res;
	}

	//!	Вывести все переменные блока
	//!	\return строка с описанием всех переменных блока
	std::set< std::string > get_var_list( bool show_locals ) const {
		std::set< std::string > res;

		if( show_locals ) {
			for( const auto & ch_p: _children ) {
				auto ch_vars = ch_p.second->get_var_list( show_locals );
				for( const auto & v: ch_vars ) {
					res.insert( ch_p.first + "." + v );
				}
			}
		} else {
			for( const auto & v_p: _vars ) {
				res.insert( v_p.first );
			}
		}
		return res;
	}

private:
	//	Список SET переменных данного блока
	std::set< std::string > _vset_names;

	//	Список USE переменных формируется автоматически при запросе переменных
	//	с помощью специальных функций
	std::set< std::string > _vuse_names;

public:

	//!	Возвращает список имен переменных SET
	const std::set< std::string > set_names( ) const noexcept
	{
		return _vset_names;
	}

	//!	Устанавливает имена переменных SET
	template< template< typename T > class Iterable >
	void set_names_set( Iterable< std::string > & names ) noexcept
	{
		for( const auto & name: names ) {
			_vset_names.insert( name );
		}
	}

	//!	Возвращает список имен переменных USE
	const std::set< std::string > use_names( ) const noexcept
	{
		return _vuse_names;
	}


private:
//	Дочерние блоки
	map_t _children;


public:
	//!	Добавить дочерний блок
	bool add_child( const std::string & name, ptr_t & bl ) {
		auto ret = _children.emplace( name, bl );
		if( ret.second == false ) {
			//	Такое имя уже присутствует в массиве
			Logger::error_invalid_block( name );
			return false;
		}
		return true;
	}

	ptr_t get_child( const std::string & name ) const {
		auto r = StringSpec::extract_block_name( name );

		auto ch = _children.find( r.first );
		if( ch == _children.end() ) {
			Logger::error_invalid_block( r.first );
			return ptr_t();
		}
		if( r.second.length() ) {
			return ch->second->get_child( r.second );
		}
		return ch->second;
	}

	//!	Вызвать дочерний блок по имени
	void call_child_by_name( const std::string & name, double time ) {

		auto it = _children.find( name );
		if( it == _children.end() ) {
			//	Такого блока нет
			Logger::error_invalid_block( name );
			return;
		}

		call_child( *it->second, time );
	}

	//!	Вызвать дочерний блок
	void call_child( block_t & bl, double time ) {

		//	Получаем список изменяемых переменных
		auto ret = bl( time, _vars );

		//	Записываем те измененные переменные, которые нужно
		write_variables( _vset_names, ret );
	}


private:
	//	Подпрограммы, которые изменяют переменные блока
	subprogram_map _subprog;


public:
	//!	Добавить подпрограмму с приоритетом
	bool add_subprog( const std::string & id, subprogram_ptr & s ) {
		auto ret = _subprog.emplace( id, s );
		if( ret.second == false ) {
			Logger::error_invalid_subprog( id );
			return false;
		}
		return true;
	}

	//!	Найти подпрограмму с заданным именем
	subprogram_ptr get_subprog( const std::string & name ) {
		//	Выделяем имя блока, в котором следует искать подпрограмму
		auto r = StringSpec::extract_block_name( name );
		if( r.second.length() ) {
			//	Указан блок, ищем во вложенном
			auto child_p = _children.find( r.first );
			if( child_p == _children.end() ) {
				Logger::error_invalid_block( r.first );
				return subprogram_ptr();
			} else {
				return child_p->second->get_subprog( r.second );
			}
		} else {
			//	Указано без блока, ищем подпрограмму в этом блоке
			auto _it = _subprog.find( name );
			if( _it == _subprog.end() ) {
				//	Такой подпрограммы нет
				Logger::error_invalid_variable( name );
				return subprogram_ptr();
			}
			return _it->second;
		}
	}

	//!	Вывести все подпрограммы блока
	//!	\return множество строк с именами всех подпрограмм блока
	std::set< std::string > get_subp_list( bool show_locals ) const {
		std::set< std::string > res;

		if( show_locals ) {
			for( const auto & ch_p: _children ) {
				auto ch_subs = ch_p.second->get_subp_list( show_locals );
				for( const auto & s: ch_subs ) {
					res.insert( ch_p.first + "." + s );
				}
			}
		} else {
			for( const auto & s_p: _subprog ) {
				res.insert( s_p.first );
			}
		}
		return res;
	}

private:
	typedef std::map< std::string, std::string > param_accordiance;

	//	Добавить переменную в словарь соответствия
	bool add_param_accordiance(
		param_accordiance & acc_map,
		const std::string & key, const std::string & value )
	{
		if( !acc_map.insert( make_pair( key, value ) ).second ) {
			//	Такая переменная уже есть в acc_map
			Logger::error_invalid_variable( key );
			return false;
		}
		return true;
	}

private:
	//	Возвращаемые переменные
	//	имя_в_блоке_уровня_выше : имя_в_текущем_блоке
	std::map< std::string, std::string > _res_vars;

public:
	//!	Добавить переменную, возвращаемую при вызове
	//!	\param src_name имя переменной источника, расположенной в этом блоке
	//!	\param dst_name имя переменной назначения, расположенной в блоке уровнем выше
	bool add_return_param
		( const std::string & src_name, const std::string & dst_name )
	{
		if( _vars.find( src_name ) == _vars.end() ) {
			//	Такой переменной нет в _vars
			Logger::error_invalid_variable( src_name );
			return false;
		}
		if( !add_param_accordiance( _res_vars, dst_name, src_name ) )
			{ return false; }

		//	Если такая переменная уже есть в _vset_names, то ничего не делаем
		_vset_names.insert( src_name );

		return true;
	}

private:
	//	Используемые параметры
	//	имя_в_текущем блоке : имя_в_блоке_уровня_выше
	std::map< std::string, std::string > _use_vars;
public:
	//!	Добавить переменную, копируемую из блока уровнем выше
	//!	\param src_name имя переменной в блоке уровнем выше
	//!	\param dst_name имя переменной в этом блоке
	bool add_use_param
		( const std::string & src_name, const std::string & dst_name )
	{
		if( _vars.find( dst_name ) == _vars.end() ) {
			//	Такой переменной нет в _vars
			Logger::error_invalid_variable( dst_name );
			return false;
		}
		return add_param_accordiance( _use_vars, dst_name, src_name );
	}

private:
	bool	_power_prev_step;	//	Наличие питания на предыдущем шаге
	parameter_map _poweron_vars;	//	Словарь переменных по включению модуля
	parameter_map _poweroff_vars;	//	Словарь переменных по выключению модуля

public:
	//!	Установить признак включения на предыдущем шаге
	void set_prev_power( bool power )
		{ _power_prev_step = power; }
	//!	Получить признак включения на предыдущем шаге
	bool get_prev_power() const
		{ return _power_prev_step; }

	//!	Установить словарь переменных, записываемых по включению модуля
	void set_poweron_vars( const parameter_map & p )
		{ _poweron_vars = p; }
	//!	Установить словарь переменных, записываемых по выключению модуля
	void set_poweroff_vars( const parameter_map & p )
		{ _poweroff_vars = p; }

	//!	Записать переменные при включении модуля
	//!	\return Обновленные возвращаемые переменные
	parameter_map write_poweron_vars() {
		const auto s = write_variables( _vset_names, _poweron_vars );
		parameter_map res;
		for( const auto & v: _res_vars ) {
			if( s.find( v.second ) != s.end() ) {
				res[ v.first ] = _poweron_vars[ v.second ]->clone();
			}
		}
		return res;
	}
	//!	Записать переменные при выключении модуля
	//!	\return Обновленные возвращаемые переменные
	parameter_map write_poweroff_vars() {
		const auto s = write_variables( _vset_names, _poweroff_vars );
		parameter_map res;
		for( const auto & v: _res_vars ) {
			if( s.find( v.second ) != s.end() ) {
				res[ v.first ] = _poweroff_vars[ v.second ]->clone();
			}
		}
		return res;
	}

private:

	//	Установить время последнего вызова в текущее
	void touch() {
		for( auto & s: _subprog ) {
			s.second->touch();
		}
		for( auto & b: _children ) {
			b.second->touch();
		}
		callable_t::touch();
		_lastcall_plan = callable_t::get_lastcall_time();
	}

private:

	//	Установить признаки питания на предыдущем шаге в 0 и установить
	//	исходное на выключение для всех дочерних блоков
	parameter_map power_off_children() {
		parameter_map res;

		for( auto & b: _children ) {
			parameter_map vm = b.second->power_off_children();
			write_variables( _vset_names, vm );
		}
		res = write_poweroff_vars();
		set_prev_power( false );

		return res;
	}

public:

	//!	Для вызова блока извне
	//!	\param vars массив переменных для использования в блоке
	//!	\param upper массив переменных блока более высокого уровня
	//! \return массив SET переменных
	parameter_map call(
			double time,
			const parameter_map & vars,
			const parameter_map & upper
		)
	{
		block_pre_call( time );

		//	Скопировать USE переменные
		for( const auto & v_name: _use_vars ) {
			//	Переменная есть, проверяли при инициализации
			_vars[ v_name.first ]->copy_data( * vars.at( v_name.second ) );
		}

		typedef typename ParamSpec< callable_t >::ptr callable_ptr;
		//	Вызывать блоки и подпрограммы в зависимости от приоритета
		auto c_obj = Scheduler::template get_nextcall_objs
				< callable_ptr, map_t, subprogram_map >
			( _children, _subprog, time );
		for( auto & c: c_obj ) {
			//	Получаем новые значения переменных
			parameter_map vm;
			try {
				auto b_p = std::dynamic_pointer_cast< block_t >( c );
				if( !b_p ) {
					//	Это подпрограмма
					if( c->is_powered( _vars ) )
						{ vm = (*c)( time, _vars, vars ); }

				} else {
					//	Это блп
					bool pow = b_p->is_powered( _vars );
					if( pow && b_p->get_prev_power() ) {
						//	Постоянная работа
						vm = (*b_p)( time, _vars, vars );
					} else {
						if( pow && !b_p->get_prev_power() ) {
							//	Передний фронт питания
							vm = b_p->write_poweron_vars();
							//	Сбрасываем время ожидания блока
							b_p->touch();
						} else if( b_p->get_prev_power() ) {
							//!	Задний фронт питания
							vm = b_p->power_off_children();
						}
					}
					b_p->set_prev_power( pow );
				}

			} catch( typename FormulaSolver::invalid_args & e ) {
				_errors.push_back(
					"Недостаточное количество операндов в выражении \""
					+ c->get_power_cond() + "\" для оператора \""
					+ e.op + "\" в позиции " + std::to_string( e.position ) );
				_err_blocks.insert( c->get_name() );
				vm.clear();
			} catch( typename FormulaSolver::invalid_operation & e ) {
				_errors.push_back(
					"Неверная операция \"" + e.operation + "\" в выражении \""
					+ c->get_power_cond() + "\"" );
				_err_blocks.insert( c->get_name() );
				vm.clear();
			} catch( exception::invalid_var_name & e ) {
				_errors.push_back(
					std::string( "Ошибка при выполнении модуля " )
					+ c->get_name() + ". Произошло обращение к "
					+ "отсутствующей переменной \"" + e.get_name() + "\"" );
				_err_blocks.insert( c->get_name() );
				vm.clear();
			} catch( std::out_of_range & e ) {
				//	Скорее всего внутри подпрограммы обратились к
				//	отсутствующей переменной
				_errors.push_back(
					std::string( "Ошибка при выполнении модуля " )
					+ c->get_name() + ". Скорее всего произошло"
					+ " обращение к отсутствующей переменной" );
				_err_blocks.insert( c->get_name() );
				vm.clear();
			} catch( exception::invalid_type & e ) {
				_errors.push_back(
					std::string( "Ошибка при выполнении модуля " )
					+ c->get_name() + ". Произошло неверное приведение "
					+ "типа для переменной \"" + e.get_name() + "\"" );
				_err_blocks.insert( c->get_name() );
				vm.clear();
			} catch( std::bad_cast & e ) {
				//	Скорее всего внутри программы вызвали неверное
				//	преобразование типа
				_errors.push_back(
					std::string( "Ошибка при выполнении модуля " )
					+ c->get_name() + ". Скорее всего произошло"
					+ " неверное приведение типа" );
				_err_blocks.insert( c->get_name() );
				vm.clear();
			} catch( subp_user_exception & e ) {
				//	Пользовательское исключение из подпрограммы
				_errors.push_back(
					std::string( "Пользовательская ошибка при работе модуля " )
					+ c->get_name() + ": \"" + e.what() + "\"" );
				_err_blocks.insert( c->get_name() );
				vm.clear();
			}

			auto bl = SubprSpec< callable_t >::
					template dynamic_ptr_cast< block_t >( c );
			if( bl ) {
				for( const auto e: bl->get_subblocks_with_errors() ) {
					_err_blocks.insert( StringSpec::compose_block_name(
							bl->get_name(), e ) );
				}
			}

			//	Записываем те новые значения, которые подходят
			write_variables( _vset_names, vm );
		}

		if( _class && _class->has_errors() ) {
			_err_blocks.insert( this->_classname );
			_errors.push_back( _class->get_error_msg() );
		}

		//	Вернуть список SET переменных
		parameter_map res;
		for( const auto & v_name: _res_vars ) {
			//	Проверку v_name осуществляем при создании множества _res_vars
			res[ v_name.first ] = _vars[ v_name.second ]->clone();
		}
		return res;
	}

public:
	std::set< std::string > get_subblocks_with_errors() {
		auto res = _err_blocks;
		_err_blocks.clear();
		return res;
	}

	std::pair< std::deque< std::string >, size_t > get_next_errors
		( size_t max=10 )
	{
		if( max >= _errors.size() ) {
			auto res = _errors; _errors.clear();
			return std::make_pair( res, 0 );
		} else {
			auto erase_beg = _errors.begin();
			auto erase_end = erase_beg + max;
			std::deque< std::string > res( erase_beg, erase_end );
			_errors.erase( erase_beg, erase_end );
			return std::make_pair( res, _errors.size() );
		}
	}

private:
	std::set< std::string > _err_blocks;
	std::deque< std::string > _errors;

};	//	class Block


}	// namespace LIBKMS_namespace


#endif

