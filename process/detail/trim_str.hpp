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

#ifndef __LIBPROCESS_DETAIL_TRIM_STR_HPP__
#define __LIBPROCESS_DETAIL_TRIM_STR_HPP__

#include <algorithm> 
#include <functional> 
#include <cctype>

// trim from start
static inline std::wstring & ltrim( std::wstring & s ) {
    s.erase( s.begin(), std::find_if_not( s.begin(), s.end(), ::isspace ) );
    return s;
}

// trim from end
static inline std::wstring & rtrim( std::wstring & s ) {
        s.erase( std::find_if_not( s.rbegin(), s.rend(), ::isspace ).base(),
			s.end() );
        return s;
}

// trim from both ends
static inline std::wstring & trim( std::wstring & s ) {
        return ltrim( rtrim( s ) );
}

#endif
