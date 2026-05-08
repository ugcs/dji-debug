#include <autopilot_errors.h>
#include <syncqueue.h>
#include <config.h>
#include <iostream>
#include <arpa/inet.h>
#include <util.h>
#include <hal.h>

#include <iostream>
#include <cstring>

#include <dji_platform.h>
#include <dji_core.h>
#include <dji_aircraft_info.h>
#include <dji_flight_controller.h>

#include <dji_low_speed_data_channel.h>
#include <dji_logger.h>
#include <m400.h>


static T_DjiAircraftInfoBaseInfo s_aircraftInfoBaseInfo;
static bool init_complete = false;


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

    //if (cfg.getSerialDevice().length() > 0) {
    //    HalUart_SetSerial(cfg.getSerialDevice().c_str(), DJI_HAL_UART_NUM_0);
    //}

    T_DjiReturnCode djiStat;
    djiStat = DjiCore_Init(&userInfo);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throwDjiError(djiStat, "Core init error: ");
    }

    djiStat = DjiAircraftInfo_GetBaseInfo(&s_aircraftInfoBaseInfo);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throwDjiError(djiStat, "Get aircraft base info error: ");
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

    T_DjiFlightControllerRidInfo ridInfo = {};

    djiStat = DjiFlightController_Init(ridInfo);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throwDjiError(djiStat, "Init flight controller module failed: ");
    }

    // Init subsctiptions
    djiStat = DjiFcSubscription_Init();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throwDjiError(djiStat, "Unit data subscription module error: ");
    }

   /*djiStat = DjiLowSpeedDataChannel_Init();
   if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
       throwDjiError(djiStat, "Init data transmission module error: ");
   }

   E_DjiChannelAddress channelAddress = DJI_CHANNEL_ADDRESS_MASTER_RC_APP;
   djiStat = DjiLowSpeedDataChannel_RegRecvDataCallback(channelAddress, onMobileDataReceived);
   if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
       throwDjiError(djiStat, "Register receive data from mobile error: ");
   }*/

   subscribeToTelemetry();

   init_complete = true;
}

void deInitInteface() {
	T_DjiReturnCode djiStat;
    if (!init_complete)
        return;

    init_complete = false;

    unsubscribeFromTelemetry();

    /*djiStat = DjiLowSpeedDataChannel_DeInit();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        std::cerr << "Failed to DeInit DataTransmision. " << buildErrorMessage(djiStat).c_str() << std::endl;
    } else {
        std::cout << "DataTransmision DeInit" << std::endl;
    }*/

    djiStat = DjiFcSubscription_DeInit();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
    	std::cerr << "Failed to DeInit Subscription. " << buildErrorMessage(djiStat).c_str() << std::endl;
    }else {
    	std::cout << "FcSubscription DeInit" << std::endl;
    }

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

T_DjiReturnCode onMobileDataReceived(const uint8_t *data, uint16_t len) {
    if (!init_complete)
        return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;

    if(len == 0)
    	return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;

  	std::vector<uint8_t> received(data, data + len);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

template<class T>
void readTopicValue(T* value, E_DjiFcSubscriptionTopic topic, T_DjiDataTimestamp& timestamp) {
    T_DjiReturnCode ret = DjiFcSubscription_GetLatestValueOfTopic(topic, reinterpret_cast<uint8_t*>(value), sizeof (T), &timestamp);
    if (ret != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throw std::runtime_error("Failed to read topic " + std::to_string(topic) + " code " + std::to_string(ret));
    }
}

std::string formatTimeStamp(const T_DjiDataTimestamp& ts) {
	uint32_t sec = ts.millisecond/1000;
	uint32_t ms = ts.millisecond %1000;
	char buf[100];
	sprintf(buf, "%u.%u", sec, ms);
	return std::string(buf);
}

void readValues()
{
	T_DjiDataTimestamp timestamp;
	T_DjiFcSubscriptionQuaternion quaternion;
	T_DjiFcSubscriptionVelocity velocity;
	T_DjiFcSubscriptionPositionFused positionFused;
	T_DjiFcSubscriptionAccelerationGround accelerationGnd;
	T_DjiFcSubscriptionAltitudeFused altitudeFused;
	T_DjiFcSubscriptionAltitudeBarometer altitudeBarometer;
	T_DjiFcSubscriptionGpsSignalLevel gpsSignal;
	T_DjiFcSubscriptionAltitudeOfHomePoint altitudeHomePoint;
	T_DjiFcSubscriptionGpsPosition gpsPosition;
	T_DjiFcSubscriptionControlDevice controlDevice;
	T_DjiFcSubscriptionFlightStatus flightStatus;
	T_DjiFcSubscriptionDisplaymode displayMode;
	T_DjiFcSubscriptionRC rc;
	T_DjiFcSubscriptionRCWithFlagData rcFlagData;
	T_DjiFcSubscriptionGpsDate gpsDate;
	T_DjiFcSubscriptionGpsTime gpsTime;
	T_DjiFcSubscriptionGpsDetails gpsDetails;
	T_DjiFcSubscriptionRtkPosition rtkPosition;
	T_DjiFcSubscriptionRtkPositionInfo rtkInfo;
	try {
		readTopicValue<T_DjiFcSubscriptionQuaternion>(&quaternion, DJI_FC_SUBSCRIPTION_TOPIC_QUATERNION, timestamp);
		printf("\r[%s]Read value", formatTimeStamp(timestamp).c_str());
	    //printf("\r[%s] Quaternion: {%f %f %f %f}", formatTimeStamp(timestamp).c_str(), quaternion.q0, quaternion.q1, quaternion.q2, quaternion.q3);

		readTopicValue<T_DjiFcSubscriptionVelocity>(&velocity, DJI_FC_SUBSCRIPTION_TOPIC_VELOCITY, timestamp);
		printf("\r[%s]Read value", formatTimeStamp(timestamp).c_str());
	    //printf("\rVelocity: {%f %f %f}. Health: %d", velocity.data.x, velocity.data.y, velocity.data.z, velocity.health);

		readTopicValue<T_DjiFcSubscriptionPositionFused>(&positionFused, DJI_FC_SUBSCRIPTION_TOPIC_POSITION_FUSED, timestamp);
		printf("\r[%s]Read value", formatTimeStamp(timestamp).c_str());

		readTopicValue<T_DjiFcSubscriptionAccelerationGround>(&accelerationGnd, DJI_FC_SUBSCRIPTION_TOPIC_ACCELERATION_GROUND, timestamp);
		printf("\r[%s]Read value", formatTimeStamp(timestamp).c_str());

		readTopicValue<T_DjiFcSubscriptionAltitudeFused>(&altitudeFused, DJI_FC_SUBSCRIPTION_TOPIC_ALTITUDE_FUSED, timestamp);
		printf("\r[%s]Read value", formatTimeStamp(timestamp).c_str());

		readTopicValue<T_DjiFcSubscriptionAltitudeBarometer>(&altitudeBarometer, DJI_FC_SUBSCRIPTION_TOPIC_ALTITUDE_BAROMETER, timestamp);
		printf("\r[%s]Read value", formatTimeStamp(timestamp).c_str());

		readTopicValue<T_DjiFcSubscriptionGpsSignalLevel>(&gpsSignal, DJI_FC_SUBSCRIPTION_TOPIC_GPS_SIGNAL_LEVEL, timestamp);
		printf("\r[%s]Read value", formatTimeStamp(timestamp).c_str());

		readTopicValue<T_DjiFcSubscriptionAltitudeOfHomePoint>(&altitudeHomePoint, DJI_FC_SUBSCRIPTION_TOPIC_ALTITUDE_OF_HOMEPOINT, timestamp);
		printf("\r[%s]Read value", formatTimeStamp(timestamp).c_str());

		readTopicValue<T_DjiFcSubscriptionGpsPosition>(&gpsPosition, DJI_FC_SUBSCRIPTION_TOPIC_GPS_POSITION, timestamp);
		printf("\r[%s]Read value", formatTimeStamp(timestamp).c_str());

		readTopicValue<T_DjiFcSubscriptionControlDevice>(&controlDevice, DJI_FC_SUBSCRIPTION_TOPIC_CONTROL_DEVICE, timestamp);
		printf("\r[%s]Read value", formatTimeStamp(timestamp).c_str());

		readTopicValue<T_DjiFcSubscriptionFlightStatus>(&flightStatus, DJI_FC_SUBSCRIPTION_TOPIC_STATUS_FLIGHT, timestamp);
		printf("\r[%s]Read value", formatTimeStamp(timestamp).c_str());

		readTopicValue<T_DjiFcSubscriptionDisplaymode>(&displayMode, DJI_FC_SUBSCRIPTION_TOPIC_STATUS_DISPLAYMODE, timestamp);
		printf("\r[%s]Read value", formatTimeStamp(timestamp).c_str());

		readTopicValue<T_DjiFcSubscriptionRC>(&rc, DJI_FC_SUBSCRIPTION_TOPIC_RC, timestamp);
		printf("\r[%s]Read value", formatTimeStamp(timestamp).c_str());

		readTopicValue<T_DjiFcSubscriptionRCWithFlagData>(&rcFlagData, DJI_FC_SUBSCRIPTION_TOPIC_RC_WITH_FLAG_DATA, timestamp);
		printf("\r[%s]Read value", formatTimeStamp(timestamp).c_str());

		readTopicValue<T_DjiFcSubscriptionGpsDate>(&gpsDate, DJI_FC_SUBSCRIPTION_TOPIC_GPS_DATE, timestamp);
		printf("\r[%s]Read value", formatTimeStamp(timestamp).c_str());

		readTopicValue<T_DjiFcSubscriptionGpsTime>(&gpsTime, DJI_FC_SUBSCRIPTION_TOPIC_GPS_TIME, timestamp);
		printf("\r[%s]Read value", formatTimeStamp(timestamp).c_str());

		readTopicValue<T_DjiFcSubscriptionGpsDetails>(&gpsDetails, DJI_FC_SUBSCRIPTION_TOPIC_GPS_DETAILS, timestamp);
		printf("\r[%s]Read value", formatTimeStamp(timestamp).c_str());
		//printf("\r[%s] GPS details: {%u}", formatTimeStamp(timestamp).c_str(), gpsDetails.gpsCounter);

		readTopicValue<T_DjiFcSubscriptionRtkPosition>(&rtkPosition, DJI_FC_SUBSCRIPTION_TOPIC_RTK_POSITION, timestamp);
		printf("\r[%s]Read value", formatTimeStamp(timestamp).c_str());

		readTopicValue<T_DjiFcSubscriptionRtkPositionInfo>(&rtkInfo, DJI_FC_SUBSCRIPTION_TOPIC_RTK_POSITION_INFO, timestamp);
		printf("\r[%s]Read value", formatTimeStamp(timestamp).c_str());
	}
	catch(std::exception& ex) {
		std::cerr << ex.what();
	}
}


