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
