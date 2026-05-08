#include <string>
#include <vector>
#include <config.h>
#include <CLI11.hpp>
#include <exception>
#include <iostream>


ParamsCLI::
ParamsCLI(int argc, char **argv)
{
	std::string app_description("Minimal sample for DJI");

	//If version number defined build string representation:
#ifdef PACKAGE_VERSION
#define APP_VERSION_(arg) " version " #arg
#define APP_VERSION(arg) APP_VERSION_(arg)

	app_description += APP_VERSION(PACKAGE_VERSION);
#endif

	CLI::App app{app_description, "DJI M400 test"};

	app.set_config("--config", "console.conf", "Load parameters from config file", false);

	app.add_option("-a, --application", app_name, "Name of registered DJI application")
				->default_val("SkyHub-V3m");

	app.add_option("-i, --id", app_id, "DJI application identifier")
			->default_val(170259);

	app.add_option("-k, --key", app_key, "Key of registered DJI application")
				->default_val("fd15f459bfeadf9cd41037839b0fff3");

	app.add_option("-l, --license", app_license, "License of registered DJI application")
				->default_val("eQ/7yCTeBUEdlTF0+eHg8K4tjKYpylWGJyCXQinsb5Pcob9KY7dBDbZ/SuovWPg2XGjYSqOIxJUGEW6nzkogtA==");

	app.add_option("-b, --baudrate", baudrate, "Serial connection baudrate")
			->default_val(921600);

	app.add_option("-e, --email", account, "Email account of registered DJI application")
				->default_val("is@ugcs.com");

	app.add_option("-s, --serial", serial_device, "Serial connection device name")
				->default_val("/dev/ttyUSB0");


	try {
		app.parse( argc, argv);
		//Update config file with new options:
		std::string conf_source = app["--config"]->as<std::string>();
		if(!conf_source.empty()) {
			std::ofstream conf_file(conf_source);
			conf_file << app.config_to_str(true, true);
			conf_file.close();
		}
	}
	catch(CLI::ParseError &e) {
		std::cerr << app.help() << std::endl;
		throw std::invalid_argument(e.what());
	}
}

std::string
ParamsCLI::getAppName() const
{
	return app_name;
}

int
ParamsCLI::getAppId() const
{
	return app_id;
}

std::string
ParamsCLI::getAppKey() const
{
	return app_key;
}

std::string
ParamsCLI::getLicense() const
{
	return app_license;
}

int
ParamsCLI::getBaudrate() const
{
	return baudrate;
}

std::string
ParamsCLI::getAccount() const
{
	return account;
}

std::string
ParamsCLI::getSerialDevice() const
{
	return serial_device;
}

