#ifndef __LIBPROCESS_SIMPLE_INSTRUCTION_PARSER_HPP__
#define __LIBPROCESS_SIMPLE_INSTRUCTION_PARSER_HPP__

#include <detail/conv_string.hpp>

#include <string>
#include <vector>
#include <boost/locale.hpp>
#include <algorithm>
#include <iostream>
#include <functional>

namespace kms {

namespace process {

struct ParseResult {
	std::string name;
	std::vector< std::string > param;

	uint32_t error_code;
	std::string error_message;

	enum {
		no_error = 0,
		invadid_instruction_name = 1,	//	Неправильное имя ФИ
		quotes_error = 2	//	Ошибка при расстановке кавычек
	};

	ParseResult():
		error_code( no_error )
	{}
};

template< typename StringDecoder >
class SimpleInstructionParser {

public:
	SimpleInstructionParser( const std::string & locale="ru_RU.UTF-8" ) {

		try{
			//	Берем локаль из имеющихся
			_loc = std::locale( locale.c_str() );
		} catch( std::runtime_error ) {
			//	Генерируем локаль, т.к. ее нет в системе
			boost::locale::generator gen;
			_loc = gen( locale.c_str() );
		}
	}

	ParseResult parse( const std::string & str ) {
		ParseResult res;

		auto next_sp = std::find_if( str.begin(), str.end(),
			[this]( char c ){ return std::isspace( c, _loc ); } );
		std::string name( str.begin(), next_sp );
		validate_name( name, res );

		while( str.end() !=
			( next_sp=extract_parameter( next_sp, str.end(), res ) ) );

		return res;
	}

private:
	//	Проверить правильность имени:
	//	Первый символ - буква, остальные - буквы, цифры или подчеркивания
	template< typename CharType=wchar_t >
	void validate_name( const std::string & name, ParseResult & res ) {
		auto name_w = _d.template decode< CharType >( name );

		auto it = name_w.begin();
		if(
			std::isalpha( *it, _loc ) &&
			( std::find_if_not( ++it, name_w.end(), [this]( CharType c ){
					return std::isalnum( c, _loc ) || c=='_';
				} ) == name_w.end() ) )
		{
			res.name = name;

		} else {
			res.error_code = ParseResult::invadid_instruction_name;
		}
	}

	//	Выделяем параметр в кавычках
	std::string::const_iterator extract_parameter(
		const std::string::const_iterator & src_start,
		const std::string::const_iterator & src_end, ParseResult & res )
	{
		//	Удаляем пробелы в начале строки
		auto next_sp = std::find_if_not( src_start, src_end,
			[this]( char c ){ return std::isspace( c, _loc ); } );

		if( next_sp == src_end ) {
			//	Больше нет параметров, возвращаем указатель на конец строки
			return src_end;
		}

		if( *next_sp == '"' ) {
			//	Параметры с кавычками, ищем до следующих не отмененных кавычек
			//	Кавычки отменяются символами '""' или '/"'
			next_sp += 1;
			auto end_par = next_sp;
			std::string par;
			while( 1 ) {
				end_par = std::find( end_par, src_end, '"' );
				if( end_par == src_end ) {
					res.error_code = ParseResult::quotes_error;
					return src_end;
				}
				if( *( end_par-1 ) == '\\' ) {
					//	Копируем кусок строки с одной кавычкой и продолжаем
					par += std::string( next_sp, end_par-1 );
					par += "\"";
					end_par++;
					next_sp = end_par;

				} else if( *( end_par+1 ) == '"' ) {
					//	Копируем кусок строки с одной кавычкой и продолжаем
					par += std::string( next_sp, end_par+1 );
					end_par += 2;
					next_sp = end_par;

				} else {
					//	Нашли подходящую кавычку, копируем строку
					par += std::string( next_sp, end_par );
					res.param.push_back( par );
					return end_par + 1;
				}
			}
		} else {
			//	Параметры без кавычек, ищем до пробела или до двойных кавычек
			auto end_par = std::find_if( next_sp, src_end, [this]( char c ) {
					return std::isspace( c, _loc ) || c=='"';
				} );

			res.param.push_back( std::string( next_sp, end_par ) );
			return end_par;
		}
		
		return next_sp;
	}

private:
	std::locale _loc;

	StringDecoder _d;
};

}	//	namespace process

}	//	namespace kms

#endif
