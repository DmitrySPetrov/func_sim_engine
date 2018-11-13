#ifndef __LIBKMS_BLOCK_CLASS_UDP_EXCHANGE_HPP__
#define __LIBKMS_BLOCK_CLASS_UDP_EXCHANGE_HPP__


#include <holder.hpp>
#include <block_class.hpp>
#include <block_class/names.hpp>
#include <block_class/inet_address.hpp>
#include <block_class/exchange_common.hpp>

#include <block_class/udp_networking.hpp>

#include <string>
#include <map>
#include <iomanip>


namespace LIBKMS_namespace{ namespace std_blcl {


template< typename T >
struct InetBuffer {
	size_t size;
	typename variable_desc< T >::list vars;

	InetBuffer(): size( 0 ) {}
	InetBuffer( size_t s ): size( s ) {}

	typedef std::map< uint16_t, InetBuffer > array;
};


template< typename T >
inline T calc_control_sum( const void * d, size_t size, T initial=0 ) {
	const T * data = static_cast< const T * >( d );
	T res = initial;
	size /= sizeof( T );
	for( size_t i=0; i<size; i++ ) {
		res += data[i];
		//	Определяем переполнение
		if( res < data[i] ) { res++; }
		if( res == 0 ) { res++; }
	}
	return res;
}


template<
	typename LogicSolver,
	typename FunctionStorage,
	typename Subprogram,
	typename SubprogramPtr,
	typename BlockPtr,
	typename ConfigNode,
	template< typename T > class BlClSpec
>
class UdpExchange:
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
	typedef UdpExchange<
			LogicSolver,
			FunctionStorage,
			Subprogram,
			SubprogramPtr,
			BlockPtr,
			ConfigNode,
			BlClSpec
		> udp_exchange_t;
	typedef typename BlClSpec< udp_exchange_t >::ptr ptr_t;

	typedef ConfigNode config_node_t;

	typedef ConfigLoader< udp_exchange_t, config_node_t >
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

	typedef typename InetBuffer< LogicSolver >::array buffer_array;

public:
	UdpExchange():
		block_class_with_config_t( BLOCK_CLASS_UDP_EXCHANGE_NAME ),
		_is_server( false ),
		_priority( 0.0 ),
		_i_counter( 0 ),
		_o_counter( 0 )
	{}

	block_class_ptr clone() const {
		return ptr_t( new udp_exchange_t( *this ) );
	}

public:

	bool read_class_config( const config_node_t & node ) {
		return config_loader_t::read_config( node, *this );
	}

	void set_address( const InetAddress & addr ) {
		_addr = addr;
	}

	void set_direction_input() { _is_server = true; }
	void set_direction_output() { _is_server = false; }

	void set_buffers( const buffer_array & buf ) {
		_buf = buf;
	}

	void set_priority( double priority ) {
		_priority = priority;
	}

	bool join_with_block( const BlockPtr & bl, const std::string & suffix ) {
		if( !validate_with_block( bl ) ) {
			this->add_error_msg( "Ошибка при проверке соответствия переменных" );
			return false;
		}

		if( _is_server ) {
			try {
				_srv = std::make_shared< udp_exchange::udp_server_caller >
					( _addr.ip, _addr.port );
			} catch( std::exception & e ) {
				std::ostringstream ss; ss << "Ошибка при создании сервера, порт "
					<< _addr.port << ": " << e.what();
				this->add_error_msg( ss.str() );
				return false;
			}
		} else {
			_clt = std::make_shared< udp_exchange::udp_client >
				( _addr.ip, _addr.port );
		}

		return block_class_t::join_with_block( bl, suffix );
	}

	SubprogramPtr get_subprog( const std::string & suffix ) {
		if( _is_server ) {
			return this->construct_subprog(
				[this]( double, double, const parameter_map &, const parameter_map & )
				{
					return input_vars();
				},
				suffix, _priority );
		} else {
			return this->construct_subprog(
				[this]( double, double, const parameter_map & v, const parameter_map & )
				{
					return output_vars( v );
				},
				suffix, _priority );
		}
	}

	parameter_map input_vars() {
		parameter_map res;

		const parameter_map & self_v = this->_vars;
		auto data = _srv->get_messages();
		for( const auto & m: data ) {
			//	Разбираем сообщения
			size_t buf_number = m[0];
			const auto & buf_desc = _buf.find( buf_number );
			if( buf_desc == _buf.end() ) {
				std::ostringstream ss;
				ss << "Принят буфер с неверным идентификатором " << m[0];
				this->add_error_msg( ss.str() );
				continue;
			}
			if( m.size() != buf_desc->second.size ) {
				std::ostringstream ss;
				ss << "Неверный размер буфера номер " << buf_number << ": "
					<< m.size();
				this->add_error_msg( ss.str() );
				continue;
			}
			//	Проверяем номер сообщения:
			//	не разрешаются только повторяющиеся сообщения
			if( static_cast< uint8_t >( _i_counter ) == m[1] ) {
				this->add_error_msg(
					"Нарушена последовательность сообщений" );
				continue;
			}
			_i_counter = m[1];
			//	Проверяем контрольную сумму
			auto cs = * reinterpret_cast< const uint16_t * >( m.data() + 2 );
			uint16_t fw = * reinterpret_cast< const uint16_t * >( m.data() );
			auto cs_expected = calc_control_sum< uint16_t >(
					m.data() + 4, m.size()-4, fw );
			if( cs_expected != cs )
			{
				std::ostringstream ss;
				ss << "Принят буфер с неверной контрольной суммой. Ожидается: "
					<< std::setbase(16) << std::setfill('0') << std::setw(4)
					<< cs_expected << ", в буфере: " << cs;
				this->add_error_msg( ss.str() );
				continue;
			}
			//	Считываем переменные
			for( const auto & v: buf_desc->second.vars ) {
				const auto v_begin = std::begin( m ) + v.offset();
				udp_exchange::msg_buffer v_data
					( v_begin, v_begin + v.size() );

				const auto tmp_it = self_v.find( v.name() );
				if( tmp_it == self_v.end() ) {
					std::ostringstream ss; ss << "Переменная \"" << v.name()
						<< "\" отсутствует в классе \""
						<< this->get_subprog_name() << "\"";
					this->add_error_msg( ss.str() );
					continue;
				}
				const auto tmp_var = tmp_it->second->clone();
				if( LIBKMS_namespace::util::from_bytes( v_data, *tmp_var ) )
				{
					res[ v.name() ] = tmp_var->clone();

				} else {
					//	Неверное значение, бракуем весь буфер
					std::ostringstream ss; ss << "Неверное значение \"";
					for( auto b: v_data ) { ss << std::hex << b; }
					ss << "\" размера " << v.size() << " для переменной \""
						<< v.name() << "\"";
					this->add_error_msg( ss.str() );
					break;
				}
			}
		}

		return res;
	}

	parameter_map output_vars( const parameter_map & u_vars )
	{
		const parameter_map & self_v = this->_vars;

		for( const auto & buf: _buf ) {
			udp_exchange::msg_buffer res( buf.second.size, 0 );
			res[0] = buf.first;

			bool isChanged = false;
			bool noError = true;

			for( const auto & var: buf.second.vars ) {
				auto u = u_vars.find( var.name() );
				if( u == u_vars.end() ) {
					std::ostringstream ss;
					ss << "В списке переменных блока отсутствует с именем \""
						<< var.name() << "\"";
					this->add_error_msg( ss.str() );
					noError = false;
					break;
				}
				auto v = self_v.find( var.name() );
				if( v == self_v.end() ) {
					std::ostringstream ss;
					ss << "В списке переменных подпрограммы отсутствует с именем \""
						<< var.name() << "\"";
					this->add_error_msg( ss.str() );
					noError = false;
					break;
				}
				isChanged = isChanged ||
					util::copy_data( * u->second, * v->second );

				udp_exchange::msg_buffer lres;
				if( var.size() != util::to_bytes( * v->second, lres ) )
				{
					std::ostringstream ss; ss << "Неверный размер переменной \""
						<< var.name() << "\"";
					this->add_error_msg( ss.str() );
					noError = false;
					break;
				}
				std::copy( lres.begin(), lres.end(),
					res.begin() + var.offset() );
			}
			//	Если не было ошибок и есть изменения
			if( noError && isChanged ) {
				//	Устанавливаем номер сообщения
				_o_counter++;
				res[1] = static_cast< uint8_t >( _o_counter );
				//	Устанавливаем контрольную сумму
				uint16_t fw = res[1]<<8 | res[0];
				auto cs = calc_control_sum< uint16_t >(
						res.data() + 4, res.size()-4, fw );
				res[2] = static_cast< uint8_t >( cs );
				res[3] = static_cast< uint8_t >( cs >> 8 );
				//	Отправить сообщение
				_clt->send_message( res );
			}
		}
		return parameter_map();
	}

private:
	bool validate_with_block( const BlockPtr & bl ) {
		for( const auto & b: _buf ) {
			for( const auto & v: b.second.vars ) {
				if( !this->validate_n_add_var( bl, v.name() ) )
					{ return false; }
			}
		}
		return true;
	}

private:
	bool _is_server;	//	прием или передача
	InetAddress _addr;	//	адрес
	buffer_array _buf;	//	буферы сообщений
	double _priority;	//	приоритет подпрограммы обмена
	size_t _i_counter;	//	счетчик входящих сообщений
	size_t _o_counter;	//	счетчик исходящих сообщений

	//	управление UDP сервером
	std::shared_ptr< udp_exchange::udp_server_caller > _srv;

	std::shared_ptr< udp_exchange::udp_client > _clt;

};	//	class UdpExchange< ... >


template<
	typename Spec,
	typename ConfigNode,
	template< typename T > class BlClSpec
>
inline bool add_UdpExchange_block_class() {
	typedef
		UdpExchange<
			typename Spec::power_on_solver_t,
			typename Spec::function_storage_t,
			typename Spec::subprogram_t,
			typename Spec::subprogram_ptr,
			typename Spec::block_ptr,
			ConfigNode,
			BlClSpec
		> udp_exchange_t;
	return Spec::block_class_factory_t::instance().\
		template add_block_class< udp_exchange_t >
			( BLOCK_CLASS_UDP_EXCHANGE_NAME );
}


}}	//	namespace LIBKMS_namespace::std_blcl


#endif

