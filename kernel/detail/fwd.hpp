#ifndef __LIBKMS_DETAIL_FWD_HPP__
#define __LIBKMS_DETAIL_FWD_HPP__

namespace LIBKMS_namespace {

template<
	typename PE,
	template< typename U > class PS
>
class Parameter;

template<
	typename T,
	typename P
>
struct Holder;

template<
	typename PE,
	template< typename U > class PS
>
class TypeFactory;

template<
	typename L,
	typename PM,
	typename FS
>
class Callable;

template<
	typename L,
	typename PM,
	typename FS
>
class Subprogram;

template<
	typename TF,
	template< typename U > class PS,
	typename S,
	typename L,
	template< typename U > class SS,
	typename StrS,
	template< typename U > class BlClSpec,
	typename FS
>
class Block;

template<
	typename ParameterMap
>
class FunctionStorage;

template<
	typename TypeFactory,
	typename BlockPtr,
	typename Logger
>
class ControlFunc;

template<
	typename TypeFactory,
	typename BlockPtr,
	typename Logger,
	template< typename U > class Spec
>
class ControlFuncStorage;


namespace std_fi {

template<
	typename TypeFactory,
	typename BlockPtr,
	typename Logger
>
class GetValue;

template<
	typename TypeFactory,
	typename BlockPtr,
	typename Logger
>
class SetValue;

template<
	typename TypeFactory,
	typename BlockPtr,
	typename Logger
>
class LockParam;

template<
	typename TypeFactory,
	typename BlockPtr,
	typename Logger
>
class GetVarList;

template<
	typename TypeFactory,
	typename BlockPtr,
	typename Logger
>
class LockSubprog;

template<
	typename TypeFactory,
	typename BlockPtr,
	typename Logger
>
class GetSubpList;

template<
	typename TypeFactory,
	typename BlockPtr,
	typename Logger
>
class LockBlock;

template<
	typename TF,
	typename BP,
	typename L
>
class ShowClassErrors;

}	//	namespace std_fi


template<
	typename FunctionStorage,
	typename Subprogram,
	typename SubprogramPtr,
	typename BlockPtr,
	template< typename T > class BlClSpec
>
class BlockClass;

template<
	typename FunctionStorage,
	typename Subprogram,
	typename SubprogramPtr,
	typename BlockPtr,
	typename ConfigNode,
	template< typename T > class BlClSpec
>
class BlockClassWithConfig;

template<
	typename FunctionStorage,
	typename Subprogram,
	typename SubprogramPtr,
	typename BlockPtr,
	template< typename U > class BlClSpec
>
class BlockClassFactory;

//!	Шаблон класса - загрузчика классов блоков
template<
	typename BlockClass,
	typename BlClLoader
>
struct ConfigLoader;


namespace std_blcl {

template<
	typename ShmObject,
	typename LogicSolver,
	typename FunctionStorage,
	typename Subprogram,
	typename SubprogramPtr,
	typename BlockPtr,
	typename ConfigLoader,
	template< typename T > class BlClSpec
>
class SharedMemoryIO;

template<
	typename LogicSolver,
	typename FunctionStorage,
	typename Subprogram,
	typename SubprogramPtr,
	typename BlockPtr,
	typename ConfigLoader,
	template< typename T > class BlClSpec
>
class UdpExchange;

}	//	namespace std_blcl


}	//	namespace LIBKMS_namespace


#endif

