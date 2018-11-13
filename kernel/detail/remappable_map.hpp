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

#ifndef __LIBKMS_DETAIL_MAPPED_MAP_HPP__
#define __LIBKMS_DETAIL_MAPPED_MAP_HPP__

#include <iostream>
#include <memory>

//!	Реализация словаря с дополнительным словарем для ключей
//!	\todo сделать собственный итератор, т.к. существующий указывает на
//!		ключи из оригинального массива, а не переименованного
template<
	typename K,
	typename V,
	template< typename ... Types > class MapType,
	template< typename ... Types > class RemapType,
	template< typename ... Types > class MapPtr
>
class remappable_map {
	typedef MapType< K, V > map_t;

public:
	//	Типы, заимствованные от словаря
	typedef K key_type;
	typedef V mapped_type;
	typedef typename map_t::value_type value_type;
	typedef typename map_t::iterator iterator;
	typedef typename map_t::const_iterator const_iterator;

private:
	//	Умный указатель на значения
	typedef MapPtr< map_t > map_ptr_t;
	map_ptr_t _p_map;

public:
	//	Методы, заимствованные от словаря

	//	Поиск данных
	iterator find( const key_type & k ) {
		return _p_map->find( _get_map_key( k ) );
	}
	const_iterator find( const key_type & k ) const {
		return _p_map->find( _get_map_key( k ) );
	}

	//	Указатель на начало словаря
	iterator begin() noexcept {
		return _p_map->begin();
	}
	const_iterator begin() const noexcept {
		return _p_map->begin();
	}

	//	Указатель на конец словаря
	iterator end() {
		return _p_map->end();
	}
	const_iterator end() const {
		return _p_map->end();
	}

	//	Разместить элемент
	template< class ... Args >
	std::pair< iterator, bool > emplace( Args && ... args ) {
		auto val = value_type( args ... );
		return _p_map->emplace( _get_map_key( val.first ), val.second );
	}

	//	Доступ к элементу
	mapped_type & operator[]( const key_type & k ) {
		return ( *_p_map )[ _get_map_key( k ) ];
	}
	mapped_type & operator[]( key_type && k ) {
		return ( *_p_map )[ _get_map_key( k ) ];
	}
	mapped_type & at( const key_type & k ) {
		return _p_map->at( _get_map_key( k ) );
	}
	const mapped_type & at( const key_type & k ) const {
		return _p_map->at( _get_map_key( k ) );
	}

	//	Удалить элемент
	void erase( const_iterator position ) {
		_p_map->erase( position );
	}

	//	Очистить массив
	void clear() noexcept {
		_p_map->clear();
	}

private:
	//	Есть ли вообще подмена имен
	bool _has_remap;

	typedef RemapType< K, K > key_map_type_t;
	//	Словарь с подменой имен: ключ - новое имя, значение - имя в словаре _map
	key_map_type_t _remap;
	key_map_type_t _inv_remap;

	//	Возвращает ключ в массиве _map
	const key_type & _get_map_key( const key_type & remapped ) const {
		if( !_has_remap ) {
			return remapped;
		} else {
			auto it = _remap.find( remapped );
			if( it == _remap.end() ) {
				return remapped;
			} else {
				return it->second;
			}
		}
	}

public:
	bool set_key_remap( const K & existed, const K & remapped ) {
		auto _p = _remap.emplace( remapped, existed );
		if( !_p.second ) {
			//	Такой ключ уже существует
			return false;
		}
		_has_remap = true;
		return true;
	}

public:
	//!	Конструктор по умолчанию
	remappable_map():
		_p_map( new map_t ),
		_has_remap( false )
	{}

	//!	Конструктор копирования должен копировать данные
	remappable_map( const remappable_map & other ):
		_p_map( new map_t( *( other._p_map ) ) ),
		_has_remap( other._has_remap ),
		_remap( other._remap )
	{}

	//!	Оператор присваивания должен копировать данные
	remappable_map & operator=( const remappable_map & other ) {
		if( this != & other ) {
			_p_map = std::make_shared< map_t >( *( other._p_map ) );
			_remap = other._remap;
			_has_remap = other._has_remap;
		}
		return *this;
	}

	//!	Использовать чужые данные
	void use_data( const remappable_map & other ) {
		_p_map = other._p_map;
	}

	//!	Возвращает массив переименования ключей
	const key_map_type_t & get_key_map() const {
		return _remap;
	}

};


#endif
