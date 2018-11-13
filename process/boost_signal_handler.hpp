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
