#include <detail/trim_str.hpp>

#include <boost_tcp_instruction_reader.hpp>
#include <boost/bind.hpp>
#include <boost/locale.hpp>

#include <iostream>

namespace kms {

namespace process {


namespace ba = boost::asio;


template< typename P >
tcp_connection::tcp_connection( ba::ip::tcp::socket socket, const P & v_n_add ):
	_socket( std::move( socket ) ),
	_validate_and_add( v_n_add )
{}

void tcp_connection::start() {
	do_read();
}

void tcp_connection::do_read() {

	auto self( shared_from_this() );
	_socket.async_read_some( ba::buffer( _data, max_data_length ),
		[this,self]( boost::system::error_code ec, std::size_t len ) {
			if( !ec ) {
				if( _validate_and_add( std::string( _data, len ) ) )
					{ do_write( "200 OK" ); }
				else
					{ do_write( "400 INVALID_STRING" ); }
			}
		} );
}

void tcp_connection::do_write( const std::string & message ) {

	auto self( shared_from_this() );
	ba::async_write( _socket, ba::buffer( message.c_str(), message.length() ),
		[this,self]( boost::system::error_code ec, std::size_t len ) {
			if( !ec ) {
				do_read();
			}
		} );
}


template< typename P >
tcp_server::tcp_server( ba::io_service & io, uint32_t port, const P & v_n_add ):
	_acceptor( io, ba::ip::tcp::endpoint( ba::ip::tcp::v4(), port ) ),
	_socket( io ),
	_validate_and_add( v_n_add )
{
	do_accept();
}

void tcp_server::do_accept() {

	_acceptor.async_accept( _socket,
		[this]( const boost::system::error_code & err ) {
			if ( !err ) {
				std::make_shared< tcp_connection >
					( std::move( _socket ), _validate_and_add )->start();
			}
			do_accept();
		} );
}


BoostTcpInstructionReader::BoostTcpInstructionReader( uint32_t port ):
	_srv( _io, port, [this]( std::string i ){ return validate_and_add( i ); } ),
	_run_thread( [this]() {
			try {
				_io.run();
			} catch( std::exception & e ) {
				std::cerr << "Ошибка: " << e.what() << std::endl;
			}
		} )
{ }

BoostTcpInstructionReader::~BoostTcpInstructionReader() {
	_io.stop();
	_run_thread.join();
}

bool BoostTcpInstructionReader::validate_and_add( std::string instr ) {

	//	TODO: Проверить инструкцию

	//	Обрезаем внешние пробелы
	std::wstring w_i = boost::locale::conv::utf_to_utf< wchar_t >( instr );
	trim( w_i );
	std::string c_i = boost::locale::conv::utf_to_utf< char >( w_i );

	_mtx.lock();
	_instr.push_back( c_i );
	_mtx.unlock();

	return true;
}

std::list< std::string > BoostTcpInstructionReader::get_instructions() {

	std::list< std::string > res;

	_mtx.lock();
	res.splice( res.end(), _instr );
	_mtx.unlock();

	return res;
}

}	//	namespace process

}	//	namespace kms
