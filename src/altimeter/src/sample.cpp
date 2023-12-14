#include <autopilot_errors.h>
#include <config.h>
#include <iostream>
#include <util.h>
#include <hal.h>

#include <iostream>
#include <cstring>

#include <dji_platform.h>
#include <dji_core.h>
#include <dji_aircraft_info.h>
#include <dji_flight_controller.h>
#include <dji_fc_subscription.h>
#include <dji_logger.h>


static T_DjiAircraftInfoBaseInfo s_aircraftInfoBaseInfo;
static bool init_complete = false;
static bool s_isM350ContactRequired = false;



void startApplication(const ParamsCLI& cfg)
{
    auto min = [](long unsigned int a, int b) {
        return static_cast<int>(a)<(b)?(a):(b);
    };

    T_DjiUserInfo userInfo = {};

    std::string s_val;
    strncpy(userInfo.appName, cfg.getAppName().c_str(), min(sizeof(userInfo.appName), cfg.getAppName().length()));
    s_val = std::to_string(cfg.getAppId());
    memcpy(userInfo.appId, s_val.c_str() , min(sizeof(userInfo.appId), s_val.length()));
    memcpy(userInfo.appKey, cfg.getAppKey().c_str(), min(sizeof(userInfo.appKey), cfg.getAppKey().length()));
    memcpy(userInfo.appLicense, cfg.getLicense().c_str(), min(sizeof(userInfo.appLicense), cfg.getLicense().length()));
    s_val = std::to_string(cfg.getBaudrate());
    memcpy(userInfo.baudRate, s_val.c_str(), min(sizeof(userInfo.baudRate), s_val.length()));
    memcpy(userInfo.developerAccount, cfg.getAccount().c_str(),min(sizeof(userInfo.developerAccount), cfg.getAccount().length()));

    if (cfg.getSerialDevice().length() > 0) {
        HalUart_SetSerial(cfg.getSerialDevice().c_str(), DJI_HAL_UART_NUM_0);
    }

    T_DjiReturnCode djiStat;
    djiStat = DjiCore_Init(&userInfo);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throwDjiError(djiStat, "Core init error: ");
    }

    djiStat = DjiAircraftInfo_GetBaseInfo(&s_aircraftInfoBaseInfo);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throwDjiError(djiStat, "Get aircraft base info error: ");
    }

    if (s_aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M350_RTK || cfg.getForceTruncate()) {
        std::cout << "M350 Type detected, truncated protocol is used";
        s_isM350ContactRequired = true;
    }

    djiStat = DjiCore_SetAlias(userInfo.appName);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throwDjiError(djiStat, "Set alias error:");
    }

    T_DjiFirmwareVersion firmwareVersion = {
        .majorVersion = 1,
        .minorVersion = 0,
        .modifyVersion = 0,
        .debugVersion = 0,
     };
    djiStat = DjiCore_SetFirmwareVersion(firmwareVersion);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throwDjiError(djiStat, "Set firmware version error: ");
    }

    djiStat = DjiCore_SetSerialNumber("PSDK12345678XX");
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throwDjiError(djiStat, "Set serial number error: ");
    }

    djiStat = DjiCore_ApplicationStart();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throwDjiError(djiStat, "Start sdk application error: ");
    }
}

void startServices() {
    T_DjiOsalHandler* osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiReturnCode djiStat;
    E_DjiChannelAddress channelAddress;
    T_DjiFlightControllerRidInfo ridInfo = {};


    djiStat = DjiFlightController_Init(ridInfo);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throwDjiError(djiStat, "Init flight controller module failed: ");
    }

    djiStat = DjiFcSubscription_Init();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throwDjiError(djiStat, "Unit data subscription module error: ");
    }

    init_complete = true;
}

void deInitInteface() {
    if (!init_complete)
        return;

    init_complete = false;

    T_DjiReturnCode djiStat;

    djiStat = DjiFlightController_DeInit();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        std::cerr << "Failed to DeInit flight controller module. " << buildErrorMessage(djiStat).c_str() << std::endl;
    }else {
        std::cout << "FlightController DeInit" << std::endl;
    }

    djiStat = DjiCore_DeInit();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        std::cerr << "Failed to DeInit DjiCore. " << buildErrorMessage(djiStat).c_str() << std::endl;
    }else {
        std::cout << "DjiCore DeInit" << std::endl;
    }
}

void subscribeAltimeterTopic() {
	T_DjiReturnCode djiStat;
	const E_DjiFcSubscriptionTopic topic = DJI_FC_SUBSCRIPTION_TOPIC_HEIGHT_FUSION;
	E_DjiDataSubscriptionTopicFreq frequency = DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ;

	DjiReceiveDataOfTopicCallback callback =
			[](const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
			-> T_DjiReturnCode {
		auto d = reinterpret_cast<const T_DjiFcSubscriptionHeightFusion*>(data);

		std::cout << "Height AGL:" << *d << " m." << std::endl;
		return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
	};

	djiStat = DjiFcSubscription_SubscribeTopic(topic, frequency, callback);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throwDjiError(djiStat, "Unit data subscription module error: ");
    }
}
