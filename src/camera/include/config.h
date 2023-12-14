#pragma once
#include <string>
#include <vector>
#include <dji_typedef.h>


class ParamsCLI {
public:
	ParamsCLI(int argc, char **argv);

	std::string getAppName() const;
	std::string getAppKey() const;
	int getAppId() const;
	std::string getLicense() const;
	int getBaudrate() const;
	std::string getAccount() const;
	std::string getSerialDevice() const;
	bool getForceTruncate() const;
	E_DjiMountPosition getCameraPosition() const;
private:
	std::string app_name;
	std::string app_key;
	std::string app_license;
	std::string account;
	int app_id;
	int baudrate;
	std::string serial_device;
	bool force_truncate;
	E_DjiMountPosition camera_position;
};
