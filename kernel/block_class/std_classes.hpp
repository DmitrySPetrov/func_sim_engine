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
