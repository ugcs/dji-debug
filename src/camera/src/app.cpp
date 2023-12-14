#include <config.h>
#include <sample.h>
#include <iostream>
#include <hal.h>


//Main entry point for application
int main(int argc, char **argv) {
	try {
		//Parse the command line parameters:
		ParamsCLI params(argc, argv);

		//Setup OSAL:
		setupEnvironment();

		//Init PSDK application:
		startApplication(params);

		//Launch PSDK tasks to send/receive data
		startServices();

		try {

		readCameraInfo();

		char cmd;
		bool run = true;
		while(run) {
			std::cout << "Enter 'p' to take a picture. Enter 'q' to stop." << std::endl;
			auto cmd = std::cin.get();
			switch(cmd) {
			case 'q':
				run = false;
				break;
			case '\n':
				break;
			default:
				std::cout << "Take photo" << std::endl;
				takePhoto();
				break;
			}
		};

		}
		catch(std::exception& ex)
		{
			std::cerr << ex.what() << std::endl;
		}

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


