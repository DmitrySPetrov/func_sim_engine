#ifndef __LIBPROCESS_DETAIL_CONV_STRING_HPP__
#define __LIBPROCESS_DETAIL_CONV_STRING_HPP__

#include <string>
#include <boost/locale.hpp>


namespace kms {

namespace process {

//	Преобразование строки с использованием boost::locale::conv
class BoostDecode {
public:

	template< typename CharType >
	std::basic_string< CharType > decode( const std::string & name )
	{
		std::basic_string< CharType > res
			= boost::locale::conv::utf_to_utf< CharType >( name );
		return res;
	}

};

}	//	namespace process

}	//	namespace kms

#endif
