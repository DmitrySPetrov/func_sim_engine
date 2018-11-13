#ifndef __LIBKMS_CLASS_SPEC_HPP__
#define __LIBKMS_CLASS_SPEC_HPP__


#include <detail/fwd.hpp>
#include <detail/std.hpp>
#include <detail/std_types.hpp>
#include <detail/spec.hpp>


namespace LIBKMS_namespace {


template<
	typename ParamExt,
	template< typename U > class ParamSpec,
	typename Scheduler,
	typename Logger,
	template< typename U > class SubprSpec,
	typename StringSpec,
	template< typename U > class BlockClassSpec,
	typename PowerOnFormulaSolver
>
struct class_spec {


typedef Parameter< ParamExt, ParamSpec > parameter_t;
typedef typename ParamSpec< parameter_t >::ptr parameter_ptr;
typedef typename ParamSpec< parameter_t >::map parameter_map;

typedef TypeFactory< ParamExt, ParamSpec > type_factory_t;
typedef Holder< std_types::tInt, parameter_t > int_holder_t;
typedef Holder< std_types::tReal, parameter_t > real_holder_t;
typedef Holder< std_types::tBool, parameter_t > bool_holder_t;

template< typename T >
struct holder{
	typedef Holder< T, parameter_t > type;
};


typedef PowerOnFormulaSolver power_on_solver_t;

typedef Callable< Logger, parameter_map, power_on_solver_t > callable_t;

typedef Block<
		type_factory_t,
		ParamSpec,
		Scheduler,
		Logger,
		SubprSpec,
		StringSpec,
		BlockClassSpec,
		PowerOnFormulaSolver
	> block_t;
typedef typename block_t::ptr_t block_ptr;
typedef typename block_t::map_t block_map;

typedef Subprogram< Logger, parameter_map, PowerOnFormulaSolver > subprogram_t;
typedef typename SubprSpec< subprogram_t >::ptr subprogram_ptr;
typedef typename SubprSpec< subprogram_t >::map subprogram_map;

typedef typename subprogram_t::user_exception subprogram_user_exception;


typedef FunctionStorage< parameter_map > function_storage_t;


typedef ControlFuncStorage< type_factory_t, block_ptr, Logger, shared_ptr_spec >
	control_func_storage_t;
typedef ControlFunc< type_factory_t, block_ptr, Logger >
	control_func_t;


typedef BlockClassFactory<
		function_storage_t,
		subprogram_t,
		subprogram_ptr,
		block_ptr,
		BlockClassSpec
	> block_class_factory_t;

typedef typename block_class_factory_t::block_class_t
	block_class_t;

};	//	struct class_spec


//!	Стандартная специализация для шаблона
typedef class_spec<
		NoExtension, remappable_shared_ptr_spec,
		StdScheduler< StdLogger >, StdLogger, shared_ptr_spec,
		StdStringSpec,
		shared_ptr_spec,
		StdPowerOnFormulaSolver
	> std_class_spec;


}	//	namespace LIBKMS_namespace


#endif
