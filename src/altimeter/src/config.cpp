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

	CLI::App app{app_description, "Embedder sonar altimeter"};

	app.set_config("--config", "console.conf", "Load parameters from config file", false);

	app.add_option("-a, --application", app_name, "Name of registered DJI application")
				->default_val("Skyhub v3");

	app.add_option("-i, --id", app_id, "DJI application identifier")
			->default_val(138744);

	app.add_option("-k, --key", app_key, "Key of registered DJI application")
				->default_val("ceda9a088de34a1a4d47277f8471953");

	app.add_option("-l, --license", app_license, "License of registered DJI application")
				->default_val("EIlyP5G2S0swBOJVLOMcTRW7AUVHimJX31q1UczI2RblKk5dhT/OOoCgzw8B2erenl86CANAdeR+F+vCy4jD8u17073YRnQy3PZxaw8JxtwA+xbxVi8CODfRtCJ4n0/XIyldeSPizDD7XZSLAixsbGenslMcjhbUSSBjwPaxSgO5xzpzppxgnT3rlAeEB1SipoLc2Mux+EtDC7I55Uvuoax2qNyMgaryrTbcQpWyfAQb33Ndm6bOfjgUC+NvEsv7v6cXKxZmw9LLiWjLM2+P9Q7E8ruW6fYq3QSR0Iq6g9bSkSt0STMafqglcNGZ92HVF0OZJZPwXWnT4ZMeIBoqGQ==");

	app.add_option("-b, --baudrate", baudrate, "Serial connection baudrate")
			->default_val(460800);

	app.add_option("-e, --email", account, "Email account of registered DJI application")
				->default_val("is@ugcs.com");

	app.add_option("-s, --serial", serial_device, "Serial connection device name")
				->default_val("/dev/ttyUSB0");

	app.add_option("-t, --force-truncate", force_truncate, "Force truncate protocol messages")
				->default_val(false);

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

bool
ParamsCLI::getForceTruncate() const
{
	return force_truncate;
}

