#include "boost_clock.hpp"

#include <boost/thread.hpp>
#include <boost/chrono.hpp>

namespace kms {

namespace process {

namespace pt = boost::posix_time;

BoostClock::BoostClock( double period ):
	_start_time( pt::microsec_clock::universal_time() ),
	_iter( pt::time_iterator( _start_time, pt::microseconds( period*1e6 ) ) )
{
	_dt = pt::microseconds( ( long )( period*1e6 ) );
}

double BoostClock::get_time() noexcept {
	pt::ptime now = pt::microsec_clock::universal_time();
	return ( now - _start_time ).total_microseconds() * 1e-6;
}

void BoostClock::save_time() {
	pt::ptime now = pt::microsec_clock::universal_time();
	_iter = pt::time_iterator( now, _dt );
}

double BoostClock::wait_interval() {
	++_iter;
	pt::ptime now = pt::microsec_clock::universal_time();
	pt::time_duration dt = *_iter - now;

	_process_time += _dt - dt;

	long dt_us = dt.total_microseconds();
	double f_dt = dt_us * 1e-6;
	if( f_dt < 0.0 ) {
		//	Не ждем, уже время прошло
		return f_dt;

	} else {
		boost::this_thread::sleep_for( boost::chrono::microseconds( dt_us ) );
		return( f_dt );
	}
}

double BoostClock::get_process_time() noexcept {
	return _process_time.total_microseconds() * 1e-6;
} 

}	//	namespace process

}	//	namespace kms
