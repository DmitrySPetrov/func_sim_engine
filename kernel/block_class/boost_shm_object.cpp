#include "block_class/boost_shm_object.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>

#include <block_class/names.hpp>


namespace bi = boost::interprocess;
namespace bpt = boost::posix_time;

namespace LIBKMS_namespace { namespace std_blcl {

template<>
const std::string & get_subprog_name< BoostShmObject >()
	{ return BLOCK_CLASS_BOOST_SHARED_MEMORY_IO_NAME; }


void BoostMappedRegionDispatcher::init
	( const BoostShmObject & shm, boost::posix_time::ptime wait_till )
{
	auto mr = shm.get_mapping_handle();

	_mtx = std::make_shared< bi::named_mutex >
			( bi::open_or_create, mr.l_name.c_str() );
	_region = bi::mapped_region( mr.shm, bi::read_write );
	_data = _region.get_address();
	_size = mr.size;

	if( !_mtx->timed_lock( wait_till ) ) {
		throw BoostMutexWaitTimeout();
	}
}

BoostMappedRegionDispatcher::BoostMappedRegionDispatcher
	( const BoostShmObject & shm, boost::posix_time::time_duration wait_for )
{
	init( shm, bpt::microsec_clock::local_time() + wait_for );
}

BoostMappedRegionDispatcher::BoostMappedRegionDispatcher
	( const BoostShmObject & shm, double wait_for )
{
	init( shm, bpt::microsec_clock::local_time() +
		boost::posix_time::microsec( long( wait_for*1e6 + 0.5 ) ) );
}

BoostMappedRegionDispatcher::~BoostMappedRegionDispatcher() {
	_mtx->unlock();
}

uint8_t * BoostMappedRegionDispatcher::get_chars( size_t offset, size_t size )
{
	if( offset + size >= _size )
		{ throw BoostMRegionInvalidAddress(); }
	return ( static_cast< uint8_t * >( _data ) + offset );
}


BoostShmObject::BoostShmObject(
		const std::string & memory_id, 
		const std::string & locker_id,
		size_t size ):
	_shm_name( memory_id ),
	_l_name( locker_id ),
	_b_open( false ),
	_size( size )
{
	try {
		//	Пробуем создавать новую память
		_shm = bi::shared_memory_object
				( bi::create_only, _shm_name.c_str(), bi::read_write );
		_b_open = false;
	} catch( bi::interprocess_exception & e ) {
		//	Память уже существует
		_shm = bi::shared_memory_object
				( bi::open_only, _shm_name.c_str(), bi::read_write );
		_b_open = true;
	}
	//	Устанавливаем размер памяти
	_shm.truncate( _size );
	//	Очищаем память
	bi::mapped_region r( _shm, bi::read_write );
	std::memset( r.get_address(), 0, r.get_size() );
}

BoostShmObject::~BoostShmObject() {
	//	Явно удаляем общую память, если она была создана нами
	if( !_b_open ) {
		boost::interprocess::shared_memory_object::remove( _shm_name.c_str() );
	}
}

BoostShmObject::mapped_handle BoostShmObject::get_mapping_handle() const {
	return mapped_handle( _shm, _size, _l_name );
}

}}	//	namespace LIBKMS_namespace::std_blcl

