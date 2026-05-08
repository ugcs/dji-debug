#include <config.h>
#include <sample.h>
#include <iostream>
#include <thread>
#include <hal.h>
#include <pthread.h>


//Main entry point for application
int main(int argc, char **argv) {
	pthread_setname_np(pthread_self(), "main_thread");
	try {
		//Parse the command line parameters:
		ParamsCLI params(argc, argv);

		//Setup OSAL:
		setupEnvironment();

		//Init PSDK application:
		startApplication(params);

		std::cout << "Sample started. Press Enter to stop." << std::endl;
		volatile bool doRead = true;
		auto readThread = std::thread([&doRead = doRead]() -> void {
			pthread_setname_np(pthread_self(), "read_telemetry");
			while(doRead){
				readValues();
			};
		});
		std::cin.get();
		doRead = false;
		readThread.join();
		//Release resources:
		deInitInteface();

		//Farewell and good luck:
		std::cout << "Work complete." << std::endl;

	}
	catch(std::invalid_argument &e) {
		std::cerr << "Incorrect argument: " << e.what() << std::endl;
		return -1;
	}
	catch(std::exception &e) {
		std::cerr << e.what() << std::endl;
		return -1;
	}

	return 0;
}


