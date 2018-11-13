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

#ifndef __LOADER_YAML_DETAIL_HPP__
#define __LOADER_YAML_DETAIL_HPP__

#include <loader_opts.hpp>

#include <yaml-cpp/yaml.h>

#include <map>
#include <string>

namespace kms {

//	Текстовые константы
const std::string const_yes = "да";
const std::string const_no = "нет";

}	//	namespace kms

namespace YAML {

//	Считать и проверить тип
template< typename T >
inline bool read_n_validate
	( const Node & node, std::string desc, NodeType::value type, T & dst )
{
	if( !node || ( node.Type() != type ) ) {
		kms::loader::serr
			<< "Пропущен или неверно указан параметр \"" << desc << "\""
			<< std::endl;
		return false;
	}
	try {
		dst = node.as< T >();
	} catch( Exception & e ) {
		kms::loader::serr << "Неверно указан параметр \"" << desc << "\""
			<< std::endl;
		return false;
	}
	return true;
}

//	Проверить, является ли словарем
inline bool validate_map( const Node & node ) {
	if( !node || !node.IsMap() ) {
		kms::loader::serr << "Неверная структура данных" << std::endl;
		return false;
	}
	return true;
}


class YesNo {
	bool _data;
public:
	YesNo( bool v ): _data( v ) {}
	YesNo(): _data( false ) {}

	operator bool() const { return _data; }

	YesNo & operator=( bool data ) { _data = data; return *this; }
};

template<>
struct convert< YesNo > {

	static bool decode( const Node & node, YesNo & d ) {
		if( !node.IsScalar() ) {
			kms::loader::serr << "Неверная структура данных" << std::endl;
			return false;
		}

		std::string data;
		if( !read_n_validate( node, "да/нет", NodeType::Scalar, data ) )
			{ return false; }
		if( data == kms::const_yes )
			{ d = true; }
		else if( data == kms::const_no )
			{ d = false; }
		else {
			kms::loader::serr << "Неверное значение \"" << d
				<< "\" ожидается \"" << kms::const_yes << "\" или \""
				<< kms::const_no << "\"" << std::endl;
			return false;
		}

		return true;
	}
};

// std::map
template<typename V>
struct convert<std::map<std::string, V> > {
	static bool decode(const Node& node, std::map<std::string, V>& rhs) {
		if(!node.IsMap()) {
			kms::loader::serr << "Неверная структура данных" << std::endl;
			return false;
		}

		rhs.clear();
		for( auto & it: node ) {
			std::string key;
			try {
				key = it.first.as<std::string>();
			} catch( Exception & e ) {
				kms::loader::serr << "Неверно указано имя" << std::endl;
				return false;
			}
			try {
				rhs[ key ] = it.second.as<V>();
			} catch( Exception & e ) {
				kms::loader::serr
					<< "Ошибка при преобразовании элемента массива \"" << key
					<< "\"" << std::endl;
				return false;
			}
		}
		return true;
	}
};

}	//	namespace YAML

#endif

