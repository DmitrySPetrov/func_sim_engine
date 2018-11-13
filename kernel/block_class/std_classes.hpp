#ifndef __LIBKMS_BLOCK_CLASS_STD_CLASSES_HPP__
#define __LIBKMS_BLOCK_CLASS_STD_CLASSES_HPP__

#include <block_class/udp_exchange.hpp>
#include <block_class/boost_shm_object.hpp>
#include <block_class/shared_memory_io.hpp>

namespace LIBKMS_namespace{ namespace std_blcl {

template<
	typename Spec,
	typename ConfigNode,
	template< typename T > class BlClSpec
>
inline bool add_std_block_classes() {
	bool res = true;
	res = res &&
		add_UdpExchange_block_class< Spec, ConfigNode, BlClSpec >();
	res = res &&
		add_SharedMemoryIO_block_class< BoostShmObject, Spec, ConfigNode, BlClSpec >();
	return res;
}

}}	//	namespace LIBKMS_namespace::std_blcl

#endif
