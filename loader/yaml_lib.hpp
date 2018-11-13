#ifndef __LOADER_YAML_LIB_HPP__
#define __LOADER_YAML_LIB_HPP__


#include <yaml_detail.hpp>
#include <yaml_names.hpp>

#include <yaml-cpp/yaml.h>


//!	Шаблон для загрузки классов блоков
namespace kms_kernel { namespace util {

template<
	typename TypeFactory,
	typename FuncStorage,
	typename BlClFactory,
	typename CntFuncFactory
>
struct library_data {
	std::string path;
	std::string init_func;
};	//	struct library_data< ... >

}}	//	namespace kms_kernel::util


namespace YAML {

template<
	typename TF,
	typename FS,
	typename BCF,
	typename CFF
>
struct convert< kms_kernel::util::library_data< TF, FS, BCF, CFF > >{

typedef kms_kernel::util::library_data< TF, FS, BCF, CFF > library_data_t;

static bool decode( const Node & node, library_data_t & d ) {

	if( !validate_map( node ) )
		{ return false; }

	if( !read_n_validate( node[ kms::lib_path ], "путь",
			NodeType::Scalar, d.path ) )
		{ return false; }

	if( !read_n_validate( node[ kms::init_func ], "функция инициализации",
			NodeType::Scalar, d.init_func ) )
		{ return false; }

	return true;
}

};	//	struct convert< ... >

}	//	namespace YAML

#endif
