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

#ifndef __LIBPROCESS_BOOSTCLOCK_HPP__
#define __LIBPROCESS_BOOSTCLOCK_HPP__

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio.hpp>

namespace kms {

namespace process {

//!	Класс часы, реализованные с помощью boost::posix_time
//!
//!	Считает моменты времени, в которые должен вызываться главный блок
//!	и возвращает моменты времени в нужном формате
class BoostClock {

public:
	//!	Конструктор часов
	//!	\param period период вызова блока в секундах
	BoostClock( double period );

	//!	Возвращает время в секундах от создания объекта
	double get_time() noexcept;

public:
	//!	Засечь текущее время для последующего выполнения шага
	void save_time();
	//	Дождаться окончания интервала, начатого при помощи save_time
	//	\return время пустого ожидания
	double wait_interval();

	//!	Вернуть полное время работы в секундах
	double get_process_time() noexcept;

private:
	boost::posix_time::ptime _start_time;

	boost::posix_time::time_iterator _iter;
	boost::posix_time::time_duration _dt;

	boost::asio::io_service _io;

	boost::posix_time::time_duration _process_time;
};

}	//	namespace process

}	//	namespace kms

#endif
