#ifndef __LOADER_YAML_PARAMETER_HPP__
#define __LOADER_YAML_PARAMETER_HPP__


#include <loader_opts.hpp>

#include <yaml-cpp/yaml.h>

#include <map>
#include <vector>
#include <string>


namespace YAML {


struct destination_desk {
	std::string destination_block;
	std::string destination_name;
};

struct parameter_desc {
	std::string name;
	std::string desc;
	std::string type;
	std::string value;
	std::string source_block;
	std::string source_name;
	std::vector< destination_desk > destination;
};

typedef std::map< std::string, parameter_desc > parameter_desc_map;


template<>
struct convert< parameter_desc > {

static bool decode( const Node & node, parameter_desc & m ) {

	Node value = node[ kms::parameter_value ];
	Node desc = node[ kms::parameter_description ];

	Node source = node[ kms::parameter_source ];
	Node destination = node[ kms::parameter_destination ];

	if( source ) {
		Node src_name = source[ kms::parameter_srcdst_variable ];
		Node src_block = source[ kms::parameter_srcdst_block ];
		//	Если не указано имя, то уровнем выше используем имя этой переменной
		if( src_name ) {
			try{
				m.source_name = src_name.as< std::string >();
			} catch( Exception & e ) {
				kms::loader::serr << "Неверно указано имя переменной-источника"
					<< std::endl;
				return false;
			}
			//	Блок всегда должен быть указан
			if( !src_block ) {
				kms::loader::serr
					<< "Укажите имя блока, порождающего переменную"
					<< std::endl;
				return false;
			}
		}
		//	Если блок указан неправильно, то разберем эту ситуацию выше
		try {
			m.source_block = src_block.as< std::string >();
		} catch( Exception & e ) {
			kms::loader::serr
				<< "Неверно указано имя порождающего переменную блока"
				<< std::endl;
			return false;
		}

		if( desc ) {
			if( !desc.IsScalar() ) {
				kms::loader::serr << "Неверно указано описание" << std::endl;
				return false;
			}
			try {
				m.desc = desc.as< std::string >();
			} catch( Exception & e ) {
				kms::loader::serr << "Неверно указано описание" << std::endl;
				return false;
			}
		}
	} else {
		//	В противном случае создаем переменную с указанным типом и значением
		Node type = node[ kms::parameter_type ];
		if( !type || !type.IsScalar() ) {
			kms::loader::serr << "Пропущен или неверно указан тип" << std::endl;
			return false;
		}
		//	Неверный тип опознаем выше
		try {
			m.type = type.as< std::string >( "" );
		} catch( Exception & e ) {
			kms::loader::serr << "Неверно указан тип" << std::endl;
			return false;
		}

		if( !value || !value.IsScalar() ) {
			kms::loader::serr << "Пропущено или неверно указано значение"
				<< std::endl;
			return false;
		}
		//	Неверное значение опознаем выше
		try {
			m.value = value.as< std::string >();
		} catch( Exception & e ) {
			kms::loader::serr << "Неверно указано значение" << std::endl;
			return false;
		}

		if( !desc || !desc.IsScalar() ) {
			kms::loader::serr << "Пропущено или неверно указано описание"
				<< std::endl;
			return false;
		}
		try {
			m.desc = desc.as< std::string >();
		} catch( Exception & e ) {
			kms::loader::serr << "Неверно указано описание" << std::endl;
			return false;
		}
	}

	if( destination ) {
		if( destination.IsSequence() ) {
			try {
				m.destination
					= destination.as< std::vector< destination_desk > >();
			} catch( Exception & e ) {
				kms::loader::serr << "Ошибка при чтении назначения переменной"
					<< std::endl;
				return false;
			}
		} else if( destination.IsMap() ) {
			try {
				m.destination.push_back( destination.as< destination_desk >() );
			} catch( Exception & e ) {
				kms::loader::serr << "Ошибка при чтении назначения переменной"
					<< std::endl;
				return false;
			}
		} else {
			kms::loader::serr << "Неверно указано назначение переменной"
				<< std::endl;
			return false;
		}
	}

	return true;
}

};	//	struct convert< parameter_desc >


template<>
struct convert< destination_desk > {

static bool decode( const Node & node, destination_desk & m ) {
	if( !node.IsMap() ) {
		kms::loader::serr << "Неверное назначение переменной " << std::endl;
		return false;
	}

	Node dst_name = node[ kms::parameter_srcdst_variable ];
	Node dst_block = node[ kms::parameter_srcdst_block ];
	//	Если не указано имя, то уровнем выше используем имя этой переменной
	if( dst_name ) {
		try {
			m.destination_name = dst_name.as< std::string >( "" );
		} catch( Exception & e ) {
			kms::loader::serr << "Ошибка при чтении имени переменной назначения"
				<< std::endl;
			return false;
		}
	}
	//	Блок всегда должен быть указан
	if( !dst_block ) {
		kms::loader::serr << "Укажите имя блока назначения для переменной"
			<< std::endl;
		return false;
	}
	//	Если блок указан неправильно, то разберем эту ситуацию выше
	try {
		m.destination_block = dst_block.as< std::string >();
	} catch( Exception & e ) {
		kms::loader::serr << "Неверно указано имя блока назначения"
			<< std::endl;
		return false;
	}

	return true;
}

};	//	struct convert< destination_desk >

}	//	namespace YAML


#endif
