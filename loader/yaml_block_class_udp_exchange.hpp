#ifndef __LOADER_YAML_BLOCK_CLASS_UDP_EXCHANGE_HPP__
#define __LOADER_YAML_BLOCK_CLASS_UDP_EXCHANGE_HPP__


#include <yaml_inet_address.hpp>

#include <yaml-cpp/yaml.h>

#include <yaml_detail.hpp>

#include <kernel.hpp>

#include <iostream>


namespace kms_kernel { namespace std_blcl {

namespace udp_exchange {

const std::string ADDRESS = "адрес";
const std::string PARAMS = "обмен";
const std::string DIRECTION = "направление";
const std::string PRIORITY = "приоритет";
const std::string BUFFERS = "буферы";
const std::string BUF_SIZE = "размер";
const std::string BUF_VARS = "переменные";
const std::string DIR_INPUT = "прием";
const std::string DIR_OUTPUT = "передача";

}	//	namespace udp_exchange

} }	//	namespace kms_kernel::std_blcl


namespace stblcl = kms_kernel::std_blcl;

namespace YAML {


template< typename T >
struct convert< stblcl::InetBuffer< T > > {

typedef stblcl::InetBuffer< T > InetBuffer;

static bool decode( const Node & node, InetBuffer & buf ) {

	using namespace kms_kernel::std_blcl::udp_exchange;

	if( !validate_map( node ) )
		{ return false; }

	if( !read_n_validate( node[ BUF_SIZE ], "размер", NodeType::Scalar,
			buf.size ) )
		{ return false; }

	if( !read_n_validate( node[ BUF_VARS ], "переменные", NodeType::Sequence,
			buf.vars ) )
		{ return false; }

	return true;
}

};


template<
	typename LS,
	typename FS,
	typename Sp,
	typename SPtr,
	typename BP,
	template< typename T > class BCS
>
struct convert< stblcl::UdpExchange< LS, FS, Sp, SPtr, BP, Node, BCS > > {

typedef stblcl::UdpExchange< LS, FS, Sp, SPtr, BP, Node, BCS > udp_exchange_t;

typedef typename stblcl::InetBuffer< LS >::array buffer_array;

static bool decode( const Node & node, udp_exchange_t & d ) {

	using namespace kms_kernel::std_blcl::udp_exchange;

	if( !validate_map( node ) )
		{ return false; }

	//	IP-адрес и порт для обмена
	stblcl::InetAddress addr;
	if( !read_n_validate( node[ ADDRESS ], "адрес", NodeType::Map, addr ) )
		{ return false; }
	d.set_address( addr );

	try {
		//	Параметры обмена
		Node params = node[ PARAMS ];
		if( !validate_map( params ) )
			{ return false; }

		std::string dir;
		if( !read_n_validate( params[ DIRECTION ], "направление",
				NodeType::Scalar, dir ) )
			{ return false; }
		if( dir == DIR_INPUT ) {
			d.set_direction_input();
		} else if( dir == DIR_OUTPUT ) {
			d.set_direction_output();
		} else {
			kms::loader::serr << "Значение \"" << dir
				<< "\" не подходит для параметра \"" << DIRECTION << "\""
				<< std::endl;
			return false;
		}

		double priority = 0.0;
		if( !read_n_validate( params[ PRIORITY ], "приоритет",
				NodeType::Scalar, priority ) )
			{ return false; }
		d.set_priority( priority );

		buffer_array buf;
		if( !read_n_validate( params[ BUFFERS ], "буферы", NodeType::Map,
				buf ) )
			{ return false; }
		d.set_buffers( buf );
	} catch( Exception & e ) {
		kms::loader::serr << "Ошибка в описании параметров обмена" << std::endl;
		return false;
	}

	return true;
}

};


}	//	namespace YAML


#endif
