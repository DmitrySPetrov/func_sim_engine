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
