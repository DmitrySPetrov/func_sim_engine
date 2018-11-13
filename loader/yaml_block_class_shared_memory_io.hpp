#ifndef __LOADER_YAML_BLOCK_CLASS_SHARED_MEMORY_IO_HPP__
#define __LOADER_YAML_BLOCK_CLASS_SHARED_MEMORY_IO_HPP__


#include <yaml-cpp/yaml.h>

#include <yaml_detail.hpp>

#include <kernel.hpp>

#include <iostream>


namespace kms_kernel { namespace std_blcl {

namespace shared_memory {

const std::string BLOCK_NAME = "имяПамяти";
const std::string LOCKER_NAME = "имяМьютекса";
const std::string SUB_PRIORITY = "приоритетПп";
const std::string PROTOCOL_NAME = "протокол";
const std::string BLOCK_DESCRIPTION = "память";
const std::string BLOCK_SIZE = "размер";
const std::string VARIABLES = "переменные";
const std::string VAR_NAME = "имя";
const std::string VAR_OFFSET = "смещение";
const std::string VAR_SIZE = "размер";
const std::string VAR_READ = "чтение";
const std::string VAR_WRITE = "запись";

}	//	namespace shared_memory

} }	//	namespace kms_kernel::std_blcl


namespace stblcl = kms_kernel::std_blcl;

namespace YAML {


template< typename T >
struct convert< stblcl::variable_desc< T > > {

typedef stblcl::variable_desc< T > variable_desc_t;

static bool decode( const Node & node, variable_desc_t & d ) {

	if( !validate_map( node ) )
		{ return false; }

	using namespace kms_kernel::std_blcl::shared_memory;

	std::string name;
	if( !read_n_validate( node[ VAR_NAME ], "имя",
			NodeType::Scalar, name ) )
		{ return false; }
	d.name( name );

	size_t offset = 0;
	if( !read_n_validate( node[ VAR_OFFSET ], "смещение",
			NodeType::Scalar, offset ) )
		{ return false; }
	d.offset( offset );

	size_t size = 0;
	if( !read_n_validate( node[ VAR_SIZE ], "размер",
			NodeType::Scalar, size ) )
		{ return false; }
	d.size( size );

	Node read = node[ VAR_READ ];
	if( read ) {
		std::string read_cond;
		if( !read_n_validate( read, "условие чтения",
				NodeType::Scalar, read_cond ) )
			{ return false; }
		d.read_cond( read_cond );
	}

	Node write = node[ VAR_WRITE ];
	if( write ) {
		std::string write_cond;
		if( !read_n_validate( write, "условие записи",
				NodeType::Scalar, write_cond ) )
			{ return false; }
		d.write_cond( write_cond );
	}

	return true;
}

};


template<
	typename Shm,
	typename LS,
	typename FS,
	typename Sp,
	typename SPtr,
	typename BP,
	template< typename T > class BCS
>
struct convert< stblcl::SharedMemoryIO< Shm, LS, FS, Sp, SPtr, BP, Node, BCS > >
{

typedef stblcl::SharedMemoryIO< Shm, LS, FS, Sp, SPtr, BP, Node, BCS >
	shared_memory_io_t;

typedef typename stblcl::variable_desc< LS >::list variable_desc_list;

static bool decode( const Node & node, shared_memory_io_t & d ) {

	if( !validate_map( node ) )
		{ return false; }

	using namespace kms_kernel::std_blcl::shared_memory;

	//	Имя блока общей памяти
	std::string memory_name;
	if( !read_n_validate( node[ BLOCK_NAME ], "имя блока общей памяти",
			NodeType::Scalar, memory_name ) )
		{ return false; }
	d.set_memory_name( memory_name );

	//	Имя объекта синхронизации
	std::string locker_name;
	if( !read_n_validate( node[ LOCKER_NAME ], "имя объекта синхронизации",
			NodeType::Scalar, locker_name ) )
		{ return false; }
	d.set_locker_name( locker_name );

	//	Приоритет подпрограммы работы с ОП
	Node n_priority = node[ SUB_PRIORITY ];
	if( n_priority ) {
		double priority = 0.0;
		if( !read_n_validate( n_priority, "приоритет подпрограммы класса",
				NodeType::Scalar, priority ) )
			{ return false; }
		d.set_priority( priority );
	}

	//	Вести протокол файла обмена
	Node n_protocol = node[ PROTOCOL_NAME ];
	if( n_protocol ) {
		std::string name;
		if( !read_n_validate( n_protocol, "протокол обмена подпрограммы класса",
				NodeType::Scalar, name ) )
			{ return false; }
		d.set_protocol_fname( name );
	}

	//	Описание структуры данных памяти
	Node memory_desc = node[ BLOCK_DESCRIPTION ];
	if( !memory_desc || !memory_desc.IsMap() ) {
		kms::loader::serr
			<< "Пропущено или неверно указано описание структуры памяти"
			<< std::endl;
		return false;
	}

	//	Размер структуры данных
	size_t memory_size;
	if( !read_n_validate( memory_desc[ BLOCK_SIZE ], "размер блока общей памяти",
			NodeType::Scalar, memory_size ) )
		{ return false; }
	d.set_memory_size( memory_size );

	//	Описание переменных в буфере
	variable_desc_list v_desk;
	if( !read_n_validate( memory_desc[ VARIABLES ], "список переменных",
			NodeType::Sequence, v_desk ) )
		{ return false; }

	for( auto v: v_desk ) {
		if( !d.add_variable_desc( v ) ) {
			kms::loader::serr << "Ошибка при добавлении переменной \""
				<< v.name() << "\"" << std::endl;
			return false;
		}
	}

	return true;
}

};	//	struct convert< kms_kernel::std_blcl::SharedMemoryIO< ... > >

}	//	namespace YAML


#endif

