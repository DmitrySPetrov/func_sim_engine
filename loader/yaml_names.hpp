#ifndef __LOADER_YAML_NAMES_HPP__
#define __LOADER_YAML_NAMES_HPP__


#include <string>


namespace kms {

//	Используемые библиотеки
const std::string libraries = "библиотеки";
const std::string lib_path = "путь";
const std::string init_func = "ф_инициализации";

//	Описание задачи
const std::string main_task_structure = "задача";

//	Описание блока
const std::string blocks = "блоки";
const std::string block_class = "класс";
const std::string block_type = "тип";
const std::string block_type_periodical = "периодический";
const std::string block_type_nonperiodical = "периодический";
const std::string block_period = "период";
const std::string block_priority = "приоритет";
const std::string block_locked_in_stop = "блокировкаВСтопе";

//	Описание переменных
const std::string parameters = "переменные";
const std::string parameter_type = "тип";
const std::string parameter_value = "значение";
const std::string parameter_description = "описание";
const std::string parameter_source = "источник";
const std::string parameter_destination = "назначение";
const std::string parameter_srcdst_block = "блок";
const std::string parameter_srcdst_variable = "переменная";

//	Описание подпрограмм
const std::string subprograms = "подпрограммы";
const std::string subprogram_name = "класс";
const std::string subprogram_priority = "приоритет";
const std::string subprogram_params = "параметры";

const std::string power_var = "условиеРаботы";

//	Описание исходных состояний
const std::string var_setup = "исходное";
const std::string var_setup_desc = "описание";
const std::string var_setup_values = "значения";
const std::string var_setup_poweron = "вкл";
const std::string var_setup_poweroff = "выкл";


}	//	namespace kms


#endif

