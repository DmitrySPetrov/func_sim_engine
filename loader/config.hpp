#ifndef __LOADER_CONFIG_HPP__
#define __LOADER_CONFIG_HPP__


#include <task.hpp>
#include <loader_opts.hpp>

#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <memory>
#include <fstream>
#include <vector>
#include <string>
#include <exception>


namespace kms {

namespace loader {


const std::string config_filename = ".kmsconfig";


struct no_task: std::exception {
	const char * what() { return "Task does not exist"; }
};


//!	Конфигурация в текущем запуске
template< typename Spec=kms_kernel::std_class_spec >
class config {

public:

	typedef task< Spec > task_t;

	typedef typename Spec::control_func_t control_func_t;
	typedef typename Spec::control_func_storage_t control_func_storage_t;

private:
	typedef std::vector< std::string >
		task_path_list;

	//	Имя задачи
	std::string _taskname;

	//	Пути поиска задачи
	task_path_list _search_paths;

	//	Путь к файлам задачи
	std::string _path;

	//	Объект - задача
	std::unique_ptr< task_t > _task;

public:

	//!	Конструктор: разбирает командную строку и загружает конфигурацию
	config( int ac, char * av[] ): _stage ( eConstructed )
	{
		if( !parse_cmdline( ac, av ) )
			{ return; }

		_search_paths.insert( _search_paths.begin(), "." );
		std::string conf_file = find_conf_file();
		if( conf_file == "" ) {
			std::cerr
				<< "Ошибка: указанные пути не содержат файла конфигурации задач"
				<< std::endl;
			return;
		}
		_stage = eTaskPathLoaded;

		if( !load_task_config( conf_file ) )
			{ return; }
		if( !_task->load() )
			{ return; }
		_stage = eReady;
	}


	//!	Вернуть загруженную задачу
	task_t & get_task() {
		if( !_task )	{
			throw no_task();
		}
		return *_task;
	}

private:
	//	Разобрать командную строку и совершить соответствующие действия
	bool parse_cmdline( int ac, char * av[] ) {

		if( !create_options_descr() )
			{ return false; }

		try {
			//	Считываем параметры командной строки
			po::variables_map vm;
			po::store( po::command_line_parser( ac, av ).
				options( _parsed_opt ).positional( _pos_opt ).run(), vm );
			po::notify( vm );

			if( vm.count( OPT_HELP ) ) {
				std::cout << _shown_opt << std::endl;
			}

			if( vm.count( OPT_VER ) ) {
				std::cout << VERSION << std::endl;
			}

		} catch( po::error & e ) {
			std::cerr << "\tProgram options error: " << e.what() << std::endl;
			return false;
		}

		if( _taskname.length() == 0 ) {
			std::cerr << "Укажите задачу для запуска" << std::endl;
			return false;
		}

		_stage = eMainConfigLoaded;
		return true;
	}

	bool create_options_descr() {

		try {
			po::options_description desc( "Загрузчик КМС\nВарианты запуска" );
			desc.add_options()
				( (OPT_HELP+",h").c_str(), "Выдает данное сообщение" )
				( (OPT_VER+",v").c_str(), "Выдает версию программы" )
			;

			po::options_description hidden;
			hidden.add_options()
				( OPT_TASK.c_str(), po::value< std::string >( &_taskname ),
					"Имя задачи для запуска" )
			;

			po::options_description config( "Конфигурация" );
			config.add_options()
				( OPT_TASK_PATH.c_str(),
					po::value< task_path_list >( &_search_paths )->composing(),
					"Пути доступа к задачам" )
			;

			_pos_opt.add( OPT_TASK.c_str(), 1 );

			_parsed_opt.add( desc ).add( config ).add( hidden );

			_shown_opt.add( desc ).add( config );

			return true;

		} catch( po::duplicate_option_error & e ) {
			//	Имена параметров дублируются
			std::cerr << "Внутренняя ошибка при разборе параметров" << std::endl;
			return false;
		}
	}

	//	Поиск задачи
	std::string find_conf_file() {

		for( const auto & d: _search_paths ) {
			try {
				fs::path p( d );
				if( !fs::is_directory( p ) ) {
					std::cout << "Ошибка: \"" << p << "\" не каталог!" << std::endl;
					continue;
				}

				p /= config_filename;
				if( fs::is_regular_file( p ) ) {
					return absolute( p ).string();
				}
			} catch( fs::filesystem_error & e ) {
				std::cerr << e.what() << std::endl;
			}
		}
		return "";
	}

	//	Загрузить параметры используемой задачи
	bool load_task_config( std::string filename ) {

		std::string path;
		std::string description;
		size_t control_port;
		size_t sig_count;
		std::ifstream fconfig( filename );

		try {
			po::options_description task_opt;
			task_opt.add_options()
				( ( _taskname+".path" ).c_str(),
					po::value< std::string >( &path ) )
				( ( _taskname+".description" ).c_str(),
					po::value< std::string >( &description ) )
				( ( _taskname+".port" ).c_str(),
					po::value< size_t >( &control_port )
						->default_value( 8001 ) )
				( ( _taskname+".sigcount" ).c_str(),
					po::value< size_t >( &sig_count )
						->default_value( 2 ) )
			;

			po::variables_map vm;
			po::store( po::parse_config_file( fconfig, task_opt, true ), vm );
			po::notify( vm );

			if( !path.length() ) {
				std::cerr << "Задача с именем '" << _taskname << "' отсутствует"
					<< std::endl;
				return false;
			}
			std::cout << "Задача \"" << _taskname << "\": " << description << std::endl
				<< "Путь к файлам задачи: " << path << std::endl
				<< "Порт управления: " << control_port << std::endl;

			_task = std::move( std::unique_ptr< task_t >
				( new task_t( path, control_port, sig_count, description ) ) );

			if( !fs::is_directory( path ) ) {
				std::cerr
					<< "Ошибка в конфигурационном файле: указанный путь модели '"
					<< path << "' не является каталогом" << std::endl;
				return false;
			}

		} catch( po::error & e ) {
			std::cerr << "\tProgram options error: " << e.what() << std::endl;
			return false;
		}

		_stage = eTaskConfigLoaded;
		return true;
	}
private:
	//	Опции, отображаемые в справочном сообщении
	po::options_description _shown_opt;
	//	Опции, передаваемые в парсер
	po::options_description _parsed_opt;
	//	Позиционные опции
	po::positional_options_description _pos_opt;

public:
	enum stage {
		eConstructed,
		eMainConfigLoaded,
		eTaskPathLoaded,
		eTaskConfigLoaded,
		eReady
	};

	stage get_stage() {
		return _stage;
	}

private:
	stage _stage;

};


//	Создать объект для парсинга
bool create_options_descr(
		po::options_description &,
		po::options_description &,
		po::options_description &
	);


}	//	namespace loader

}	//	namespace kms


#endif
