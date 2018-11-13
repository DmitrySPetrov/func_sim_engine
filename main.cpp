//!	Главная функция загрузчика

//!	Вызов программы:
//!	  kmsload -v -- показать версию программы
//!	  kmsload -? -- 


#include <kernel.hpp>
#include <process.hpp>
#include <loader.hpp>


#include <iostream>
#include <functional>
#include <string>


int main( int ac, char * av[] ) {

	namespace kl = kms::loader;

	typedef kl::config<> config;

	config conf( ac, av );
	if( conf.get_stage() != config::eReady )
		{ return 1; }
	config::task_t & t = conf.get_task();	//	Ссылка на задачу

	typedef kms::process::Dispatcher<
			kms::process::BoostTcpInstructionReader,
			kms::process::BoostClock,
			kms::process::BoostSignalHandler,
			kms::process::SimpleInstructionParser< kms::process::BoostDecode >
		>
		dispatcher;

	try {
		dispatcher dis( t.get_main_block(), t.get_control_port(),
				t.get_sig_count() );
		dis.run();

		double full_time = dis.get_process_time();
		uint32_t num_steps = dis.get_steps_total();
		std::cout << "Среднее время выполнения шага: " << full_time/num_steps
			<< "; шагов: " << num_steps << std::endl;
		std::cout << "Полное время работы: " << full_time << std::endl;

	} catch( std::runtime_error & e ) {
		std::cerr << "Ошибка при работе диспетчера модели: " << e.what()
			<< std::endl;
	}
	return 0;
}

