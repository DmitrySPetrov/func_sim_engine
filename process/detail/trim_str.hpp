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
