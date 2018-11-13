#include "boost_system_io.hpp"

namespace kms { namespace process {

BoostSystemIo::BoostSystemIo():
	_io( new boost::asio::io_service() )
{};

}}	//	namespace kms::process

