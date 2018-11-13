#ifndef __LIBKMS_BLOCK_CLASS_SHARED_MEMORY_IO_HPP__
#define __LIBKMS_BLOCK_CLASS_SHARED_MEMORY_IO_HPP__


#include <block_class.hpp>
#include <block_class/names.hpp>
#include <block_class/exchange_common.hpp>

#include <string>
#include <sstream>
#include <cstring>
#include <fstream>

#include <boost/chrono/io/time_point_io.hpp>


namespace LIBKMS_namespace {

namespace std_blcl {


template< typename ShmObject >
const std::string & get_subprog_name();


template<
	typename ShmObject,
	typename LogicSolver,
	typename FunctionStorage,
	typename Subprogram,
	typename SubprogramPtr,
	typename BlockPtr,
	typename ConfigNode,
	template< typename T > class BlClSpec
>
class SharedMemoryIO:
	public BlockClassWithConfig<
			FunctionStorage,
			Subprogram,
			SubprogramPtr,
			BlockPtr,
			ConfigNode,
			BlClSpec
		>
{
public:
	typedef SharedMemoryIO<
			ShmObject,
			LogicSolver,
			FunctionStorage,
			Subprogram,
			SubprogramPtr,
			BlockPtr,
			ConfigNode,
			BlClSpec
		> shared_memory_io_t;
	typedef typename BlClSpec< shared_memory_io_t >::ptr ptr_t;

	typedef ConfigNode config_node_t;

	typedef ConfigLoader< shared_memory_io_t, config_node_t >
		config_loader_t;

	typedef BlockClass<
			FunctionStorage,
			Subprogram,
			SubprogramPtr,
			BlockPtr,
			BlClSpec
		> block_class_t;

	typedef BlockClassWithConfig<
			FunctionStorage,
			Subprogram,
			SubprogramPtr,
			BlockPtr,
			ConfigNode,
			BlClSpec
		> block_class_with_config_t;

	typedef typename block_class_t::ptr_t block_class_ptr;

	typedef typename block_class_t::parameter_map parameter_map;

	typedef enum { create_only, open_only, open_or_create }
		create_or_open_t;

	typedef variable_desc< LogicSolver > variable_desc_t;

public:
	SharedMemoryIO():
		block_class_with_config_t( get_subprog_name< ShmObject >() ),
		_m_size( 0 ),
		_is_writable( false ),
		_create_or_open( open_only ),
		_priority( 0.0 )
	{}

	block_class_ptr clone() const {
		return ptr_t( new shared_memory_io_t( *this ) );
	}

public:
	//!	Установить имя участка памяти
	void set_memory_name( const std::string & m_name ) {
		_m_name = m_name;
	}

	//!	Установить имя объекта блокировки
	void set_locker_name( const std::string & l_name ) {
		_l_name = l_name;
	}

	//!	Установить размер памяти
	void set_memory_size( size_t m_size ) {
		_m_size = m_size;
	}

	//!	Можно ли писать в память
	void set_writable_flag( bool is_writable ) {
		_is_writable = is_writable;
	}

	//!	Создать или открыть память
	void set_creation_policy( create_or_open_t p ) {
		_create_or_open = p;
	}

	//!	Установить приоритет функции работы с памятью
	void set_priority( double priority ) {
		_priority = priority;
	}

	//!	Установить имя файла-протокола
	void set_protocol_fname( const std::string & name ) {
		_protocol_fname = name;
	}

	bool add_variable_desc( const std::string & var,
			size_t offset, size_t size, bool read, bool write )
	{
		_m_var_offset.push_back
			( variable_desc_t( var, offset, size, read, write ) );
		return true;
	}

	bool add_variable_desc( variable_desc_t desc )
	{
		_m_var_offset.push_back( desc );
		return true;
	}

	bool read_class_config( const config_node_t & node ) {
		return config_loader_t::read_config( node, *this );
	}

	bool join_with_block( const BlockPtr & bl, const std::string & suffix ) {
		if( !validate_with_block( bl ) ) {
			this->add_error_msg(
				"Ошибка при проверке соответствия переменных" );
			return false;
		}

		try {
			//	Создаем общую память
			_shm = std::shared_ptr< ShmObject >
					( new ShmObject( _m_name, _l_name, _m_size ) );
		} catch( std::exception & e ) {
			this->add_error_msg(
				"Ошибка при инициализации объекта общей памяти: "
				+ std::string( e.what() ) );
			return false;
		}

		return block_class_t::join_with_block( bl, suffix );
	}

	//!	Возвращает ссылку на подпрограмму чтения/записи общей памяти
	SubprogramPtr get_subprog( const std::string & suffix ) {
		return this->construct_subprog(
			[this]( double, double,
					const parameter_map & v, const parameter_map & )
			{
				return shared_memory_io( v );
			},
			suffix, _priority );
	}

	//!	Подпрограмма для чтения из общей памяти и записи в общую память
	//	TODO: вынести проверку условий чтения/записи в параметр шаблона
	parameter_map shared_memory_io( const parameter_map & v ) {
		parameter_map res;

		parameter_map _v = v;

		//	Заблокировать память и получить указатель
		typename ShmObject::mapped_region_t r( *_shm, 0.01 );

		std::ostringstream log;
		bool has_log = !_protocol_fname.empty();

		//	Прочитать/записать
		for( const auto & var: _m_var_offset ) {
			try {
				uint8_t * bytes =
					r.get_chars( var.offset(), var.size() );

				auto & par = _v.at( var.name() );

				if( var.do_read( _v ) ) {
					auto p = par->clone();
					p->from_bytes( bytes, var.size() );
					res[ var.name() ] = p->clone();
					par = p->clone();
					if( has_log ) {
						log << "=== Read at "
							<< boost::chrono::time_fmt
									( boost::chrono::timezone::local )
							<< boost::chrono::system_clock::now()
							<< " ===" << std::endl
							<< var.name() << ":\t" << p->to_string()
							<< std::endl << std::endl;
					}
				}

				if( var.do_write( _v ) ) {
					memset( bytes, 0, var.size() );
					auto out_buffer = par->to_bytes();
					if( var.size() < out_buffer.size() ) {
						this->add_error_msg(
							"Размера буфера недостаточно для записи переменной "
							+ var.name() );
					} else {
						memcpy( bytes, par->to_bytes().data(),
							out_buffer.size() );
						if( has_log ) {
							log << "=== Write at "
								<< boost::chrono::time_fmt
										( boost::chrono::timezone::local )
								<< boost::chrono::system_clock::now()
								<< " ===" << std::endl
								<< var.name() << ":\t" << par->to_string()
								<< std::endl << std::endl;
						}
					}
				}

			} catch( std::out_of_range & e ) {
				this->add_error_msg( "Ошибка при получении переменной "
					+ var.name() );
			} catch( std::exception & e ) {
				this->add_error_msg( std::string( "Ошибка при" )
					+ " обработке переменной " + var.name()
					+ " в общей памяти."
					+ " Проверь смещение переменной и условие чтения/записи" );
			}
		}

		if( has_log ) {
			std::ofstream flog( _protocol_fname, std::ofstream::app );
			flog << log.str();
			flog.close();
		}

		return res;
	}

private:
	bool validate_with_block( const BlockPtr & bl ) {
		for( const auto & v: _m_var_offset ) {
			if( !this->validate_n_add_var( bl, v.name() ) )
				{ return false; }
		}
		return true;
	}

private:
	std::string _m_name;
	std::string _l_name;
	size_t _m_size;
	bool _is_writable;
	create_or_open_t _create_or_open;
	std::shared_ptr< ShmObject > _shm;
	double _priority;	//	приоритет подпрограммы обмена
	std::string _protocol_fname;	//	протокол обмена

private:
	typename variable_desc_t::list _m_var_offset;

};	//	class SharedMemoryIO


template<
	typename ShmObject,
	typename Spec,
	typename ConfigNode,
	template< typename T > class BlClSpec
>
bool add_SharedMemoryIO_block_class() {
	typedef
		SharedMemoryIO<
			ShmObject,
			typename Spec::power_on_solver_t,
			typename Spec::function_storage_t,
			typename Spec::subprogram_t,
			typename Spec::subprogram_ptr,
			typename Spec::block_ptr,
			ConfigNode,
			BlClSpec
		> shared_memory_io_t;
	return Spec::block_class_factory_t::instance().\
		template add_block_class< shared_memory_io_t >
			( get_subprog_name< ShmObject >() );
}


}	//	namespace std_blcl

}	//	namespace LIBKMS_namespace


#endif

