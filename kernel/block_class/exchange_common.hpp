////////////////////////////////////////////////////////////////////////////////
// 
//  This file is part of Functional Simulation Engine.
//
//  Functional Simulation Engine is free software: you can redistribute it
//  and/or modify it under the terms of the GNU General Public License as
//  published by the Free Software Foundation, either version 3 of the License,
//  or (at your option) any later version.
//
//  Functional Simulation Engine is distributed in the hope that it will be
//  useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
//  Public License for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with Functional Simulation Engine.
//  If not, see <https://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __LIBKMS_BLOCK_CLASS_EXCHANGE_COMMON_HPP__
#define __LIBKMS_BLOCK_CLASS_EXCHANGE_COMMON_HPP__


#include <string>
#include <list>


namespace LIBKMS_namespace { namespace std_blcl {


template< typename LogicSolver >
class variable_desc {
	std::string _name;
	size_t	_offset;
	size_t	_size;
	LogicSolver _read_cond;
	LogicSolver _write_cond;

public:
	variable_desc(): _offset( 0 ), _size( 0 ) {}

public:
	size_t size() const { return _size; }
	size_t offset() const { return _offset; }
	std::string name() const { return _name; }

	template< typename Map >
	bool do_read( const Map & m ) const
		{ return _read_cond.solve( m ); }

	template< typename Map >
	bool do_write( const Map & m ) const
		{ return _write_cond.solve( m ); }

	void size( size_t s ) { _size = s; }
	void offset( size_t o ) { _offset = o; }
	void name( const std::string & n ) { _name = n; }
	void read_cond( const std::string & rs )
		{ _read_cond = LogicSolver( rs, true ); }
	void write_cond( const std::string & ws )
		{ _write_cond = LogicSolver( ws, true ); }

public:
	typedef std::list< variable_desc< LogicSolver > >list;
};

}}	//	namespace LIBKMS_namespace::std_blcl

#endif
