#ifndef __LIBPROCESS_BOOST_SIGNAL_HANDLER_HPP__
#define __LIBPROCESS_BOOST_SIGNAL_HANDLER_HPP__

#include <boost/asio.hpp>

#include <functional>

namespace kms { namespace process {

//!	Класс обработки сигналов SIGINT и SIGTERM
//!	Использовать для перехвата Ctrl+C
class BoostSignalHandler{
public:
	typedef std::function< void( int ) > signal_handler_t;

public:
	//! Конструктор класса
	//!	\param handler функция обрабоки сигнала
	BoostSignalHandler( signal_handler_t handler );

private:
	boost::asio::io_service _io;
	boost::asio::signal_set _sig;
	signal_handler_t _h;

private:
	void async_wait();
};

}}	//	namespace kms::process

#endif
