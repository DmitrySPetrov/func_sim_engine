#ifndef __LIBKMS_BLOCK_CLASS_BOOST_SHARED_MEMORY_HPP__
#define __LIBKMS_BLOCK_CLASS_BOOST_SHARED_MEMORY_HPP__

#include <detail/namespace.hpp>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>


namespace LIBKMS_namespace { namespace std_blcl {

//	Ранняя декларация
template< typename ShmObject >
const std::string & get_subprog_name();


struct BoostShmObject;

struct BoostMutexWaitTimeout: std::exception {
	const char * what() const noexcept
	{
		return "timeout elapsed";
	}
};

struct BoostMRegionInvalidAddress: std::exception {
	const char * what() const noexcept
	{
		return "invalid address";
	}
};


class BoostMappedRegionDispatcher {
private:
	void init(
			const BoostShmObject & shm,
			boost::posix_time::ptime wait_till );

public:
	BoostMappedRegionDispatcher(
			const BoostShmObject & shm,
			boost::posix_time::time_duration wait_for );

	BoostMappedRegionDispatcher(
			const BoostShmObject & shm,
			double wait_for_sec );

	~BoostMappedRegionDispatcher();

public:
	//	Представить в виде массива элементов определенного типа
	template< typename T >
	std::vector< T > as_vector() {
		size_t size = _size / sizeof( T );
		auto p = static_cast< T * >( _region );
		return std::vector< T >( p, p+size );
	}

	uint8_t * get_chars( size_t offset, size_t size );

	template< typename T >
	T at( size_t offset ) {
		if( offset + sizeof( T ) >= _size )
			{ throw BoostMRegionInvalidAddress(); }
		void * p = static_cast< char * >( _region ) + _size;
		return * static_cast< T * >( p );
	}

	size_t size() const
		{ return _size; }


private:
	std::shared_ptr< boost::interprocess::named_mutex > _mtx;
	boost::interprocess::mapped_region _region;

	void * _data;
	size_t _size;
};


class BoostShmObject {
public:
	typedef struct mapped_handle_t {
			const boost::interprocess::shared_memory_object & shm;
			size_t size;
			std::string l_name;
			mapped_handle_t(
					const boost::interprocess::shared_memory_object & sh,
					size_t s, const std::string & l
				): shm( sh ), size( s ), l_name( l ) {}
			
		} mapped_handle;

	typedef BoostMappedRegionDispatcher mapped_region_t;

public:
	BoostShmObject(
			const std::string & memory_id,
			const std::string & locker_id,
			size_t size );

	~BoostShmObject();

public:
	mapped_handle get_mapping_handle() const;

private:
	//	Имя объекта общей памяти
	std::string _shm_name;
	std::string _l_name;

	//	Была ли память открыта или создана
	bool _b_open;

	//	Размер общей памяти
	size_t _size;

	//	Объект общей памяти
	boost::interprocess::shared_memory_object _shm;

};


}}	//	namespace LIBKMS_namespace::std_blcl

#endif
