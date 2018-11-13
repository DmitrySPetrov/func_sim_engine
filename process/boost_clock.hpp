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
