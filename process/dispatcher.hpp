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

#ifndef __LIBPROCESS_DISPATCHER_HPP__
#define __LIBPROCESS_DISPATCHER_HPP__

#include <detail/proc_spec.hpp>

#include <list>
#include <set>
#include <string>
#include <iostream>
#include <atomic>

namespace kms {

namespace process {

//!	Шаблонный класс - диспетчер модели
//!
//!	Обеспечивает запуск модели, считывание и исполнение инструкций (ФИ)
template<
	typename InstructionReader,
	typename Clock,
	typename SignalHandler,
	typename InstructionParser
>
class Dispatcher {
public:
	//!	Конструктор диспетчера
	//!	\param main_block указатель на главный блок задачи
	Dispatcher( Spec::block_ptr main_block, uint32_t port, size_t sigcount ):
		_main_block( main_block ),
		_clock( _main_block->get_period() ),
		_ir( port ),
		_sh( [this]( int snum ){ handle_SIGINT_SIGTERM( snum ); } ),
		_steps_total( 0 ),
		_running( false ),
		_sig_count( sigcount )
	{}

	//!	Выполнить вызов блока и подождать время до окончания шага
	//!	\return время ожидания
	double run_once( ) {
		make_step();
		return _clock.wait_interval();
	}

	//!	Запустить на выполнение
	void run() {
		_running = true;
		while( _running ) {
			run_once();
		}
	}

	//!	Выполнить указанное количество шагов
	void run_steps( std::size_t num_steps ) {
		for( std::size_t s = 0; s<num_steps; s++ ) {
			run_once();
		}
	}

	//!	Получить количество вызовов главного блока
	uint32_t get_steps_total() noexcept
		{ return _steps_total; }

	//!	Получить полное время работы главного блока
	double get_process_time() noexcept
		{ return _clock.get_process_time(); }

private:

	std::set< std::string > _errors;

	//	Считать и выполнить инструкции, вызывать главный блок
	void make_step() {
		double time = _clock.get_time();

		//	Считать инструкции
		std::list< std::string > instr = _ir.get_instructions();

		//	Разобрать инструкции
		for( const auto & s: instr ) {
			std::stringstream output;
			output << "*" << s << " : " << time << std::endl;

			if( s == "краб" ) {
				_running = false;
			} else if( s == "halt" ) {
				_running = false;
			} else if( s == "инфраб" ) {
				double full_time = get_process_time();
				output << "Среднее время выполнения шага: "
					<< full_time/_steps_total << "; шагов: "
					<< _steps_total << std::endl;
				output << "Полное время работы: " << full_time << std::endl;
			} else {
				auto res = _ip.parse( s );

				//	Буфер содержит описание вызова ФИ.
				//	Очищаем его и составляем из расшифрованных данных
				output.str( "" );
				output << "*" << res.name;
				for( auto & p: res.param ) {
					output << " " << p;
				}
				output << " : " << time << std::endl;

				auto func_p =
					Spec::control_func_storage_t::instance().get( res.name );
				if( !func_p ) {
					output << "Нет ФИ \"" << res.name << "\"" << std::endl;

				} else if( !func_p->set_parameter_values( res.param ) ) {
					output << "Неверное количество параметров для ФИ \""
						<< res.name << "\": указано " << res.param.size()
						<< ", требуется " << func_p->get_parameter_count()
						<< std::endl;

				} else {
					output << func_p->run( _main_block );
				}
			}
			std::cout << std::endl << output.str();
		}

		//	Вычислить главный блок
		( *_main_block )( time );

		for( const auto & e: _main_block->get_subblocks_with_errors() ) {
			if( _errors.insert( e ).second ) {
				std::cout << "Имеется ошибка в исполняемом модуле \"" << e
					<< "\"" << std::endl;
			}
		}
		
		_steps_total++;
	}

private:
	Spec::block_ptr _main_block;
	Clock _clock;
	InstructionReader _ir;
	InstructionParser _ip;
	SignalHandler _sh;

	uint32_t _steps_total;

	std::atomic< bool > _running;

private:
	std::atomic< int > _sig_count;

	void handle_SIGINT_SIGTERM( int sig ) {
		std::cout << "Получен сигнал ";
		if( sig == SIGINT ) {
			std::cout << "SIGINT";
		} else if( sig == SIGTERM ) {
			std::cout << "SIGTERM";
		} else {
			std::cout << sig;
		}
		std::cout << ". ";
		--_sig_count;
		if( _sig_count > 0 ) {
			std::cout << "Для завершения повторите " << _sig_count << " раз";
		} else {
			std::cout << "Завершение работы...";
			_running = false;
		}
		std::cout << std::endl;
	}
};

}	//	namespace process

}	//	namespace kms


#endif
