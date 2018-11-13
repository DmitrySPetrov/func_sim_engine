#ifndef __LIBKMS_PARAM_CONVERT_HPP__
#define __LIBKMS_PARAM_CONVERT_HPP__


#include <detail/namespace.hpp>
#include <param/fwd.hpp>


namespace LIBKMS_namespace {


//!	Шаблонный класс для преобразования параметров в нужный тип
template< typename T, typename P >
struct obj_as {

static T & as( P & obj ) {
	return dynamic_cast< T & >( obj );
}

static const T & c_as( const P & obj ) {
	return dynamic_cast< const T & >( obj );
}

};	//	template< typename T > struct obj_as


}	//	namespace LIBKMS_namespace


#endif
