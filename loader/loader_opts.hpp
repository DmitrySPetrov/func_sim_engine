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

