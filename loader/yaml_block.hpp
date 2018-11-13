#ifndef __LOADER_YAML_BLOCK_HPP__
#define __LOADER_YAML_BLOCK_HPP__


#include <yaml-cpp/yaml.h>

#include <yaml_detail.hpp>

#include <kernel.hpp>

#include <yaml_names.hpp>
#include <yaml_parameter.hpp>
#include <yaml_subpr.hpp>

#include <string>
#include <memory>
#include <map>


//!	Шаблон для загрузки классов блоков
namespace kms_kernel {

template< typename BlockClass >
struct ConfigLoader< BlockClass, YAML::Node > {

typedef YAML::Node node_t;
typedef BlockClass block_class_t;

typedef ConfigLoader< block_class_t, node_t >
	config_loader_t;

static bool read_config( const node_t & node, block_class_t & data ) {

	return YAML::convert< block_class_t >::decode( node, data );
}

};	//	struct ConfigLoader<...>

}	//	namespace kms_kernel


namespace YAML {


typedef struct _tStateD {
		std::string desc;
		std::map< std::string, std::string > vars;
	} tStateD;
template<>
struct convert< tStateD > {

static bool decode( const Node & node, tStateD & st ) {
	if( !validate_map( node ) )
		{ return false; }
	if( !read_n_validate( node[ kms::var_setup_desc ], "описание",
			NodeType::Scalar, st.desc ) )
		{ return false; }
	if( !read_n_validate( node[ kms::var_setup_values ], "значения",
			NodeType::Map, st.vars ) )
		{ return false; }

	return true;
}

};


template<
	typename TF,
	template< typename U > class PS,
	typename S,
	typename L,
	template< typename U > class SS,
	typename StrS,
	template< typename U > class BCS,
	typename FS
>
class convert< std::shared_ptr<
		kms_kernel::Block< TF, PS, S, L, SS, StrS, BCS, FS > > >
{
	typedef kms_kernel::Block< TF, PS, S, L, SS, StrS, BCS, FS > block_t;
	typedef typename block_t::ptr_t block_ptr;
	typedef typename block_t::map_t block_map;

	typedef TF type_factory_t;
	typedef typename type_factory_t::parameter_ptr parameter_ptr;
	typedef typename PS< typename type_factory_t::parameter_t >::map
		parameter_map;

	typedef typename kms_kernel::Subprogram< L, parameter_map, FS >
		subprogram_t;
	typedef typename SS< subprogram_t >::ptr subprogram_ptr;
	typedef typename SS< subprogram_t >::map subprogram_map;

	typedef kms_kernel::BlockClassFactory<
			kms_kernel::FunctionStorage< parameter_map >,
			subprogram_t,
			subprogram_ptr,
			block_ptr,
			BCS
		> block_class_factory_t;

	typedef typename block_class_factory_t::
			template block_class_with_config< Node >::type
		block_class_with_config_t;
	typedef typename BCS< block_class_with_config_t >::ptr
		block_class_with_config_ptr;

	typedef typename block_class_factory_t::block_class_t
		block_class_t;
	typedef typename BCS< block_class_t >::ptr
		block_class_ptr;
			
public:

	static bool decode( const Node & node, block_ptr & bl ) {

		if( !validate_map( node ) )
			{ return false; }

		//	Класс блока
		std::string class_s;
		if( !read_n_validate( node[ kms::block_class ], "класс",
				NodeType::Scalar, class_s ) )
			{ return false; }

		//	Тип блока
		std::string s_type;
		if( !read_n_validate( node[ kms::block_type ], "тип", NodeType::Scalar,
				s_type ) )
			{ return false; }
		if( s_type != kms::block_type_periodical
			&& s_type != kms::block_type_nonperiodical )
		{
			kms::loader::serr
				<< "Неверно указан тип, ожидается значение \""
				<< kms::block_type_periodical << "\" или \""
				<< kms::block_type_nonperiodical << "\""<< std::endl;
			return false;
		}

		//	Период блока
		double v_period;
		if( !read_n_validate( node[ kms::block_period ], "период",
				NodeType::Scalar, v_period ) )
			{ return false; }

		//	Приоритет блока
		double v_priority;
		if( !read_n_validate( node[ kms::block_priority ], "приоритет",
				NodeType::Scalar, v_priority ) )
			{ return false; }

		//	Блокировка в стопе
		YesNo s_locked_in_stop;
		if( !read_n_validate( node[ kms::block_locked_in_stop ],
				"признак блокировки в стопе", NodeType::Scalar,
				s_locked_in_stop ) )
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

		//	Параметры блока
		parameter_desc_map p_map;
		{
			Node params = node[ kms::parameters ];
			//	Параметров может и не быть
			if( params && !read_n_validate( params, "массив переменных",
					NodeType::Map, p_map ) )
				{ return false; }
		}

		//	Дочерние блоки
		block_map child_map;
		{
			Node blocks = node[ kms::blocks ];
			//	Блоков может не быть
			if( blocks && !read_n_validate( blocks, "массив блоков",
					NodeType::Map, child_map ) )
				{ return false; }
		}

		//	Подпрограммы
		subprogram_map subs_map;
		{
			Node subps = node[ kms::subprograms ];
			//	Подпрограмм может не быть
			if( subps && !read_n_validate( subps, "массив подпрограмм",
					NodeType::Map, subs_map ) )
				{ return false; }
		}

		//	Исходные состояния
		std::map< std::string, tStateD > state_map;
		{
			Node pset = node[ kms::var_setup ];
			//	Исходных может не быть
			if( pset && !read_n_validate( pset, "исходное",
					NodeType::Map, state_map ) )
				{ return false; }
		}

		//	Создаем блок
		//	Временно устанавливаем имя блока в XXX
		bl = std::make_shared< block_t >( class_s, "XXX" );
		if( s_type == kms::block_type_periodical )
			{ bl->make_periodical(); }
		bl->set_period( v_period );
		bl->set_priority( v_priority );
		bl->set_blocked_in_stop( s_locked_in_stop );
		if( !bl->set_power_cond( v_power ) ) {
			kms::loader::serr << "Ошибка при добавлении условия выполнения \""
				<< v_power << "\"" << std::endl;
			return false;
		}
		for( auto & p_p: p_map ) {
			parameter_ptr p;
			if( p_p.second.source_block.empty() ) {
				//	Своя переменная, не копируем из дочернего блока
				p = type_factory_t::instance().create( p_p.second.type );
				if( !p ) {
					//	Такого типа переменной нет
					kms::loader::serr << "Тип переменной \""
						<< p_p.second.type << "\" отсутствует" << std::endl;
					return false;
				}
				p->from_string( p_p.second.value );
			} else {
				//	Копируем из дочернего блока
				auto child_p = child_map.find( p_p.second.source_block );
				if( child_p == child_map.end() ) {
					//	Такого блока нет
					kms::loader::serr << "Дочерний блок \""
						<< p_p.second.source_block
						<< "\" указанный как порождающий переменную \""
						<< p_p.first << "\" отсутствует" << std::endl;
					return false;
				}
				auto & bl = *( child_p->second );
				const std::string & p_name =
					( p_p.second.source_name.empty() )
						? ( p_p.first ) : ( p_p.second.source_name );
				auto p_var = bl.get_variable( p_name );
				if( !p_var ) {
					//	Такой переменной нет в блоке
					kms::loader::serr << "Переменная \""
						<< p_p.second.source_block << "." << p_name
						<< "\" отсутствует" << std::endl;
					return false;
				}
				p = p_var->clone();
				//	Сообщаем о копировании
				if( !bl.add_return_param( p_name, p_p.first ) ) {
					//	Такой переменной нет в блоке
					kms::loader::serr << "Ошибка при задании \""
						<< p_p.second.source_block << "." << p_name
						<< "\" в качестве источника переменной \""
						<< p_p.first << "\"" << std::endl;
					return false;
				}
			}

			//	Устанавливаем признак копирования в дочерний блок
			if( !p_p.second.destination.empty() ) {
				for( auto d: p_p.second.destination ) {
					//	Ищем дочерний блок
					auto child_p = child_map.find( d.destination_block );
					if( child_p == child_map.end() ) {
						//	Такого блока нет
						kms::loader::serr << "Дочерний блок \""
							<< d.destination_block
							<< "\", указанный как использующий переменную \""
							<< p_p.first << "\" отсутствует" << std::endl;
						return false;
					}
					auto & bl = *( child_p->second );
					//	Ищем переменную
					const std::string & p_name =
						( d.destination_name.empty() )
							? ( p_p.first ) : ( d.destination_name );
					auto p_var = bl.get_variable( p_name );
					if( !p_var ) {
						//	Такой переменной нет в блоке
						kms::loader::serr << "Переменная \""
							<< d.destination_block << "." << p_name
							<< "\" отсутствует" << std::endl;
						return false;
					}
					//	Проверяем соответствие типов
					if( p_var->get_type() != p->get_type() ) {
						kms::loader::serr << "Типы переменных \""
							<< d.destination_block << "." << p_name << "\" и \""
							<< p_p.first << "\" не совпадают" << std::endl;
						return false;
					}
					//	Сообщаем о копировании
					if( !bl.add_use_param( p_p.first, p_name ) ) {
						kms::loader::serr << "Ошибка при задании \""
							<< p_p.second.source_block << "." << p_var
							<< "\" в качестве назначения переменной \""
							<< p_p.first << "\"" << std::endl;
						return false;
					}
				}
			}

			//	Устанавливаем описание
			if( !p_p.second.desc.empty() )
				{ p->set_desc( p_p.second.desc ); }
			
			if( !bl->add_variable( p_p.first, p ) ) {
				kms::loader::serr << "Ошибка при добавлении переменной \""
					<< p_p.first << "\"" << std::endl;
				return false;
			}
		}
		for( auto & c_b: child_map ) {
			c_b.second->set_name( c_b.first );
			//	Проверяем, есть ли pow-переменная дочернего блока в текущем
			auto pvs = c_b.second->get_power_vars();
			for( auto vs: pvs ) {
					if( p_map.find( vs ) == p_map.end() ) {
					kms::loader::serr << "Переменная - условие работы \""
						<< vs << "\" для блока \""
						<< c_b.first << "\" не существует" << std::endl;
					return false;
				}
			}
			//	Добавляем блок
			if( !bl->add_child( c_b.first, c_b.second ) ) {
				kms::loader::serr << "Ошибка при добавлении дочернего блока \""
					<< c_b.first << "\"" << std::endl;
				return false;
			}
		}
		for( auto & s_i: subs_map ) {
			s_i.second->set_name( s_i.first );
			//	Проверяем, есть ли pow-переменная дочернего блока в текущем
			auto pvs = s_i.second->get_power_vars();
			for( auto vs: pvs ) {
				if( p_map.find( vs ) == p_map.end() ) {
					kms::loader::serr << "Переменная - условие работы \""
						<< vs << "\" для подпрограммы \""
						<< s_i.first << "\" не существует" << std::endl;
					return false;
				}
			}
			//	Добавляем подпрограмму
			if( !bl->add_subprog( s_i.first, s_i.second ) ) {
				kms::loader::serr << "Ошибка при добавлении подпрограммы \""
					<< s_i.first << "\"" << std::endl;
				return false;
			}
		}

		//	Исходные значения переменных
		for( auto s: state_map ) {
			//	TODO: реализовать исходные с произвольными именами
			if( s.first != kms::var_setup_poweron
					&& s.first != kms::var_setup_poweroff )
			{
				kms::loader::serr << "Недопустимое имя исходного \""
					<< s.first << "\"" << std::endl;
				return false;
			}
			parameter_map setup;
			for( auto p: s.second.vars ) {
				auto p_it = p_map.find( p.first );
				if( p_it == p_map.end() ) {
					kms::loader::serr << "Ошибка при добавлении исходного \""
						<< s.first << "\": параметр \"" << p.first
						<< "\" отсутствует в блоке" << std::endl;
					return false;
				}
				auto p_p = bl->get_variable( p.first )->clone();
				if( !p_p->from_string( p.second ) ) {
					kms::loader::serr << "Недопустимое значение \""
						<< p.second << "\" для параметра \"" << p.first << "\""
						<< std::endl;
					return false;
				}
				setup.emplace( p.first, p_p );
			}

			if( s.first == kms::var_setup_poweron ) {
				bl->set_poweron_vars( setup );
			} else if( s.first == kms::var_setup_poweroff ) {
				bl->set_poweroff_vars( setup );
			}
		}

		if( class_s.length() > 0 ) {
			block_class_with_config_ptr blcl =
				block_class_factory_t::template with_config< Node >
					( bl->get_block_class() );
			if( !blcl ) {
				kms::loader::serr << "Указан несуществующий класс \""
					<< class_s << "\"" << std::endl;
				return false;
			}
			//	Считываем конфигурации класса блока
			if( !blcl->read_class_config( node ) ) {
				kms::loader::serr << "Ошибка при чтении конфигурации класса \""
					<< class_s << "\"" << std::endl;
				return false;
			}

			if( !blcl->join_with_block( bl ) ) {
				kms::loader::serr
					<< "Ошибка при создании класса блока. "
					<< blcl->get_error_msg()
					<< std::endl;
				return false;
			}
		}

		return true;
	}

};	//	struct convert< std::shared_ptr< kms_kernel::Block< T, Y > > >

}	//	namespace YAML

#endif
