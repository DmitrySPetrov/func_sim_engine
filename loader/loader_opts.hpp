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

#ifndef __LOADER_KMSOPTS_HPP__
#define __LOADER_KMSOPTS_HPP__

#include <string>
#include <iostream>


namespace kms {

namespace loader {


const std::string OPT_HELP = "help";
const std::string OPT_VER = "version";
const std::string OPT_TASK = "task";

const std::string OPT_TASK_PATH = "task-path";


//	Версии нумеруются:
//	  <старшая версия>.<младшая версия>.<год подверсии>.<месяц подверсии>
const std::string VERSION = "0.1.14.6";


extern std::ostream & serr;


}	//	namespace loader

}	//	namespace kms


#endif

