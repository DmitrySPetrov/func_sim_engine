#ifndef __LOADER_YAML_SUBPR_HPP__
#define __LOADER_YAML_SUBPR_HPP__


#include <kernel.hpp>
#include <yaml_names.hpp>

#include <yaml-cpp/yaml.h>


namespace YAML {

template< typename T, typename E, typename FS >
class convert< std::shared_ptr< kms_kernel::Subprogram< T, E, FS > > > {

	typedef kms_kernel::Subprogram< T, E, FS > subprogram_t;
	typedef std::shared_ptr< subprogram_t > subprogram_ptr;

public:

	static bool decode( const Node & node, subprogram_ptr & s ) {
		if( !validate_map( node ) )
			{ return false; }

		//	Имя функции
		std::string s_func;
		if( !read_n_validate( node[ kms::subprogram_name ], "имя функции",
				NodeType::Scalar, s_func ) )
			{ return false; }

		//	Приоритет блока
		double s_priority;
		if( !read_n_validate( node[ kms::subprogram_priority ], "приоритет",
				NodeType::Scalar, s_priority ) )
			{ return false; }

		//	Условие работы (имя переменной)
		std::string v_power;
		{
			Node n = node[ kms::power_var ];
			//	Может быть не указана
			if( n && !read_n_validate( n, "условие работы",
					NodeType::Scalar, v_power ) )
				{ return false; }
		}

		std::map< std::string, std::string > var_map;
		{
			Node n = node[ kms::subprogram_params ];
			//	Может быть не указано
			if( n && !read_n_validate( n, "параметры",
					NodeType::Map, var_map ) )
				{ return false; }
		}

		s = std::make_shared< subprogram_t >( node.Tag(), s_func, s_priority );
		if( !s->is_func_set() ) {
			kms::loader::serr << "Функция \"" << s_func << "\" не определена"
				<< std::endl;
			return false;
		}
		if( !s->set_power_cond( v_power ) ) {
			kms::loader::serr << "Ошибка при добавлении условия выполнения \""
				<< v_power << "\"" << std::endl;
			return false;
		}
		for( const auto & p: var_map ) {
			if( !s->set_key_remap( p.second, p.first ) ) {
				kms::loader::serr << "Ошибка при добавлении отображения переменной \""
					<< p.second << "->" << p.first << "\"" << std::endl;
				return false;
			}
		}

		return true;
	}

};	//	struct convert< kms_kernel::Subprogram< T > >

}	//	namespace YAML


#endif
