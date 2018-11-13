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

#ifndef __LOADER_LOAD_LIB_HPP__
#define __LOADER_LOAD_LIB_HPP__

#include <string>
#include <iostream>


#ifndef _WIN32
#include <dlfcn.h>
#else
#include <windows.h>
#endif


namespace kms {

namespace loader {

//	Функция для загрузки библиотеки подпрограмм
template< typename TF, typename FS, typename BCF, typename CFS >
bool load_library( std::string f_name, std::string init_func ) {

	typedef void( *init_func_t )( TF *, FS *, BCF *, CFS * );

	//	Ищем библиотеку с таким именем
#ifndef _WIN32
	void * lib = dlopen( f_name.c_str(), RTLD_LAZY );
	if( lib == NULL ) {
		//	Такой библиотеки нет
		char * error = dlerror();
		std::cerr << "Ошибка при загрузке библиотеки '" << f_name << "': "
			<< error << std::endl;
		return false;
	}
#else
	HINSTANCE lib = LoadLibrary( TEXT( f_name.c_str() ) );
	if( lib == NULL ) {
		std::cerr << "Ошибка при загрузке библиотеки '" << f_name << "': "
			<< GetLastError() << std::endl;
		return false;
	}
#endif

	//	Запускаем функцию инициализации
	if( !init_func.empty() ) {
		init_func_t init;
#ifndef _WIN32
		init = reinterpret_cast< init_func_t >
					( dlsym( lib, init_func.c_str() ) );
		char * error;
		if( ( error = dlerror() ) != NULL ) {
			std::cerr << "Ошибка при загрузке функции '" << init_func
				<< "': " << error << std::endl;
			return false;
		}
#else
		init = ( init_func_t )( GetProcAddress( lib, init_func.c_str() ) );
		if( init == NULL ) {
			std::cerr << "Ошибка при загрузке функции '" << init_func << "': "
				<< GetLastError() << std::endl;
			return false;
		}
#endif

		init(
				& TF::instance(),
				& FS::instance(),
				& BCF::instance(),
				& CFS::instance()
			);
	}
	return true;
}



}	//	namespace loader

}	//	namespace kms


#endif
