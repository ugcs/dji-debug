#include <ping-pong.h>
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
#include <dji_low_speed_data_channel.h>
#include <dji_flight_controller.h>
#include <dji_logger.h>


static T_DjiAircraftInfoBaseInfo s_aircraftInfoBaseInfo;
static bool init_complete = false;
static bool s_useAsyncSend = true;
static bool s_sendTaskRunning = true;
static bool s_isM350ContactRequired = false;
static SyncQueue<std::vector<uint8_t>> s_msgQueue;
static T_DjiTaskHandle s_userDataTransmissionThread;



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

     djiStat = DjiLowSpeedDataChannel_Init();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throwDjiError(djiStat, "Init data transmission module error: ");
    }

    channelAddress = DJI_CHANNEL_ADDRESS_MASTER_RC_APP;
    djiStat = DjiLowSpeedDataChannel_RegRecvDataCallback(channelAddress, onMobileDataReceived);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throwDjiError(djiStat, "Register receive data from mobile error: ");
    }

    if (s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1 ||
        s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_PAYLOAD_PORT_NO2 ||
        s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_PAYLOAD_PORT_NO3) {
        channelAddress = DJI_CHANNEL_ADDRESS_EXTENSION_PORT;
        djiStat = DjiLowSpeedDataChannel_RegRecvDataCallback(channelAddress, onMobileDataReceived);
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            throwDjiError(djiStat, "Register receive data from onboard coputer error: ");
        }
    } else if (s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_EXTENSION_PORT) {
        channelAddress = DJI_CHANNEL_ADDRESS_PAYLOAD_PORT_NO1;
        djiStat = DjiLowSpeedDataChannel_RegRecvDataCallback(channelAddress, onMobileDataReceived);
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            throwDjiError(djiStat, "Register receive data from payload NO1 error: ");
        }
    } else {
        throw std::runtime_error("Failed to install data receiver");
    }

    if (osalHandler->TaskCreate("TransportSendTask", UserDataTransmission_Task,
                                2048, NULL, &s_userDataTransmissionThread) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throwDjiError(djiStat, "User data transmission task create error: ");
    }

    djiStat = DjiFlightController_Init(ridInfo);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throwDjiError(djiStat, "Init flight controller module failed: ");
    }

    init_complete = true;
}

void deInitInteface() {
    if (!init_complete)
        return;

    init_complete = false;
    s_sendTaskRunning = false;
    const std::vector<uint8_t> empty_msg;
    s_msgQueue.enqueue(empty_msg); // Place single message to notify

    T_DjiOsalHandler* osalHandler = DjiPlatform_GetOsalHandler();
    osalHandler->TaskSleepMs(1000);

    T_DjiReturnCode djiStat;
    djiStat = osalHandler->TaskDestroy(s_userDataTransmissionThread);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        std::cerr << "Failed to Destroy DataTransmission Task. " << buildErrorMessage(djiStat).c_str() << std::endl;
    } else {
        std::cout << "DataTransmission Task destroyed" << std::endl;
    }

    djiStat = DjiLowSpeedDataChannel_DeInit();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        std::cerr << "Failed to DeInit DataTransmision. " << buildErrorMessage(djiStat).c_str() << std::endl;
    } else {
        std::cout << "DataTransmision DeInit" << std::endl;
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

    if(s_isM350ContactRequired) {
        std::vector<uint8_t> received(data+1, data + len - 1);
        receivedData(received);
    }
    else {
    	std::vector<uint8_t> received(data, data + len);
    	receivedData(received);
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

void receivedData(const std::vector<uint8_t>& data) {
    if (!init_complete)
        return;

    if (data.size() > C::MaxFrameLength) {
        std::cout << "Frame length from mobile SDK is too big:" << data.size();
        return;
    }

    parseIncomingData(data);
}

void parseIncomingData(const std::vector<uint8_t> &data)
{
	//WARNING: This code does not support endiannes.
	//Since UgCS protocol is little-endiannes, this natively works only for x86-64 platform!!!
	//TODO: add endianness correction

	if(data.size() < OnboardMessageHeader::size)
		return;
	const OnboardMessageHeader* header = reinterpret_cast<const OnboardMessageHeader*>(data.data());

	switch(header->id)
	{
	case C::PingMessageIndex:
	{
		const PingPayload* ping = reinterpret_cast<const PingPayload*>(data.data() + OnboardMessageHeader::size);
		std::cout << "Got ping message with TS=" << ping->timeStampMs << std::endl;
		sendPong(ping->timeStampMs);
		break;
	}
	default:
		break;
	}
}

void sendPong(uint64_t timestamp)
{
	struct {
		OnboardMessageHeader header = {C::PongMessageIndex, PongMessage::size};
		PongMessage message;
	} msg;

	msg.message.timestamp = timestamp;
	msg.message.major = C::VSM_PROTOCOL_VERSION_MAJOR;
	msg.message.minor = C::VSM_PROTOCOL_VERSION_MINOR;

	//WARNING: This code does not support endiannes.
	//Since UgCS protocol is little-endiannes, this natively works only for x86-64 platform!!!
	//TODO: add endianness correction

    std::vector<uint8_t> data;
    data.resize(OnboardMessageHeader::size + PongMessage::size + sizeof(uint16_t));
    uint8_t* buff = data.data();

    //Serialize the struct field-by-field:
    std::memcpy(buff, &msg.header.id, sizeof(msg.header.id));
    buff += sizeof(msg.header.id);
    std::memcpy(buff, &msg.header.length, sizeof(msg.header.length));
    buff += sizeof(msg.header.length);
    std::memcpy(buff, &msg.message.timestamp, sizeof(msg.message.timestamp));
    buff += sizeof(msg.message.timestamp);
    std::memcpy(buff, &msg.message.major, sizeof(msg.message.major));
    buff += sizeof(msg.message.major);
    std::memcpy(buff, &msg.message.minor, sizeof(msg.message.minor));
    buff += sizeof(msg.message.minor);
    uint16_t crc = calcCrc(data.data(), buff-data.data());
    std::memcpy(buff, &crc, sizeof(crc));
    buff += sizeof(crc);

    std::cout << "Send pong with TS=" << timestamp << std::endl;

    sendDataToMSDK(data);
}

uint16_t calcCrc(const void* data, size_t length)
{
	uint16_t crc = 0xFFFF;
    uint16_t tmp;
    const uint8_t *u8 = reinterpret_cast<const uint8_t *>(data);
    for (size_t i = 0; i < length; ++i) {
        tmp = u8[i] ^ (crc & 0xFF);
        tmp ^= (tmp << 4) & 0xFF;
        crc = ((crc >> 8) & 0xFF) ^ static_cast<uint16_t>((tmp << 8) ^ (tmp << 3) ^ ((tmp >> 4) & 0xFF));
    }
    return crc;
}

void sendDataToMSDK(const std::vector<uint8_t>& data) {
    if (!init_complete)
        return;

    std::vector<uint8_t> toSend(data.begin(), data.end());

    if (s_isM350ContactRequired)
        toSend.push_back(0x01);

    if (s_useAsyncSend) {
        s_msgQueue.enqueue(toSend);
    } else {
        T_DjiReturnCode djiStat = DjiLowSpeedDataChannel_SendData(DJI_CHANNEL_ADDRESS_MASTER_RC_APP, toSend.data(), toSend.size());
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
            std::cerr << "Send data to MSDK error: " << buildErrorMessage(djiStat).c_str() << std::endl;
    }
}

void* UserDataTransmission_Task(void *) {
    T_DjiReturnCode djiStat;

    while (s_sendTaskRunning) {
        auto elem = s_msgQueue.dequeue();
          if(elem.size() > 0) {
          djiStat = DjiLowSpeedDataChannel_SendData(DJI_CHANNEL_ADDRESS_MASTER_RC_APP, (uint8_t*)elem.data(), elem.size());
          if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
            std::cerr << "Send data to low speed channel error: " << buildErrorMessage(djiStat).c_str();
        }
    }
    return nullptr;
}
