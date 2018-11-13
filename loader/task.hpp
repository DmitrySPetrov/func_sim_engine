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

#ifndef __LOADER_TASK_HPP__
#define __LOADER_TASK_HPP__


#include <yaml_block_class_shared_memory_io.hpp>
#include <yaml_block_class_udp_exchange.hpp>
#include <yaml_lib.hpp>
#include <yaml_block.hpp>
#include <yaml_names.hpp>

#include <load_lib.hpp>

#include <kernel.hpp>
#include <block_class/std_classes.hpp>

#include <yaml-cpp/yaml.h>

#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;

#include <string>
#include <list>


namespace kms {

namespace loader {


//	Имена файлов с конфигурацией
const std::string task_config_file = "config.yml";


template< typename Spec >
class task {
public:
	typedef typename Spec::block_ptr block_ptr;
	typedef typename Spec::block_class_factory_t block_class_factory_t;
	typedef typename Spec::type_factory_t type_factory_t;
	typedef typename Spec::function_storage_t function_storage_t;
	typedef typename Spec::control_func_storage_t control_func_storage_t;

	typedef kms_kernel::util::library_data<
			type_factory_t,
			function_storage_t,
			block_class_factory_t,
			control_func_storage_t
		> library_data_t;

public:
	//!	Создать объект - задачу
	task(
			std::string path, size_t port,
			size_t sigcount=2, std::string description=""
		):
			_path( path ),
			_description( description ),
			_port( port ),
			_sigcount( sigcount )
	{}

	//!	Загрузить из формата YAML
	bool load() {
		fs::path conf_path( _path );
		conf_path /= task_config_file;

		try {
			YAML::Node config = YAML::LoadFile( conf_path.string() );

			//	Инициализируем фабрику классов блоков
			kms_kernel::std_blcl::add_std_block_classes
					< Spec, YAML::Node, kms_kernel::shared_ptr_spec >();

			//	Загружаем нужные библиотеки
			std::list< library_data_t > ld;
			if( !read_n_validate( config[ libraries ], "библиотеки",
					YAML::NodeType::Sequence, ld ) )
				{ return false; }
			for( auto l: ld ) {
				if( !load_library<
							type_factory_t,
							function_storage_t,
							block_class_factory_t,
							control_func_storage_t
						>( l.path, l.init_func ) )
					{ return false; }
			}

			//	Загружаем главный блок
			if( !read_n_validate( config[ main_task_structure ], "задача",
					YAML::NodeType::Map, _main_block ) )
				{ return false; }
			_main_block->set_name( main_task_structure );

			std::cout << "Период: " << _main_block->get_period() << std::endl;

			return true;

		} catch( YAML::ParserException & e ) {
			std::cerr << "Ошибка: " << e.what() << std::endl;
		} catch( YAML::BadConversion & e ) {
			std::cerr << "Ошибка преобразования типа при загрузке блока '"
				<< main_task_structure << "': " << e.what() << std::endl;
		} catch( YAML::BadFile & e ) {
			std::cerr << "Ошибка: " << e.what() << std::endl;
		}

		return false;
	}


private:
	std::string _path;
	std::string _description;
	size_t _port;
	size_t _sigcount;

private:
	//	Главный блок задачи
	block_ptr _main_block;

public:
	//!	Возвращает ссылку на главный блок задачи
	block_ptr get_main_block() {
		return _main_block;
	}

	//!	Возвращает порт управления
	size_t get_control_port() {
		return _port;
	}

	//!	Возвращает количество сигналов SIGINT и SIGTERM, которые
	//!	задача ожидает до выхода
	size_t get_sig_count() {
		return _sigcount;
	}
};


}	//	namespace loader

}	//	namespace kms


#endif

