#ifndef __LIBKMS_SUBPROG_HPP__
#define __LIBKMS_SUBPROG_HPP__


#include <callable.hpp>
#include <function_storage.hpp>

#include <exception>


namespace LIBKMS_namespace {


//!	Шаблонный класс - обертка подпрограмм
template<
	typename Logger,
	typename ParameterMap,
	typename FormulaSolver
>
class Subprogram: public Callable< Logger, ParameterMap, FormulaSolver > {

	std::string _func_name;

public:
	typedef ParameterMap parameter_map;

	typedef FunctionStorage< ParameterMap > function_storage_t;

public:
	struct user_exception: std::exception {
		user_exception( const std::string & data, const std::string & desc ):
			_data( data ),
			_desc( desc )
		{}

		const char * what() const noexcept {
			if( _data.length() > 0 ) {
				std::string res = _desc + "; ошибочный набор данных: \""
						+ _data + "\"";
				return res.c_str();
			} else {
				return _desc.c_str();
			}
		}
	private:
		const std::string _data;
		const std::string _desc;
	};

public:
	parameter_map call( double time,
		const parameter_map & v, const parameter_map & u )
	{
		const auto * f = function_storage_t::instance().get( _func_name );
		if( f ) {
			_remapped.use_data( v );
			auto res = ( * f )( time, this->get_delta_time(), _remapped, u );
			for( auto p: _remapped.get_key_map() ) {
				auto it = res.find( p.first );
				if( it != res.end() ) {
					res.emplace( p.second, it->second );
					res.erase( it );
				}
			}
			return res;
		} else {
			return parameter_map();
		}
	}

	Subprogram( std::string name, std::string class_, double priority ):
		Callable< Logger, ParameterMap, FormulaSolver >
			( name, priority, false ), _func_name( class_ )
	{
		const auto * f = function_storage_t::instance().get( _func_name );
		if( !f ) {
			Logger::error_invalid_subprog( _func_name );
		}
	}

private:
	parameter_map _remapped;
	typedef typename parameter_map::key_type param_key_t;

public:

	bool set_key_remap(
			const param_key_t & target,
			const param_key_t & remapped
		)
	{
		return _remapped.set_key_remap( target, remapped );
	}

	bool is_func_set() const noexcept {
		return function_storage_t::instance().get( _func_name );
	}

};


}	//	namespace LIBKMS_namespace


#endif
