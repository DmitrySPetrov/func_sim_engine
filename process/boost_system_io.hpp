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

#ifndef __LIBPROCESS_SYSTEM_IO_HPP__
#define __LIBPROCESS_SYSTEM_IO_HPP__

#include <boost/asio.hpp>

#include <memory>

namespace kms { namespace process {

class BoostSystemIo {
public:
	typedef std::shared_ptr< boost::asio::io_service > io_ptr;

public:
	BoostSystemIo();
		
public:
	io_ptr get_io();

private:
	io_ptr _io;
};

}}	//	namespace kms::process

#endif
