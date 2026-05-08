#include <m400.h>
#include <iostream>



static const std::map<E_DjiFcSubscriptionTopic, TopicSubsction> topicSubscriptions = {
	{DJI_FC_SUBSCRIPTION_TOPIC_QUATERNION, {"Quaternion", DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ, nullptr}},
    {DJI_FC_SUBSCRIPTION_TOPIC_VELOCITY, {"Velocity", DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ, nullptr}},
    {DJI_FC_SUBSCRIPTION_TOPIC_POSITION_FUSED, {"Position", DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ, nullptr}},
    {DJI_FC_SUBSCRIPTION_TOPIC_ACCELERATION_GROUND, {"Acceleration", DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ, nullptr}},
    {DJI_FC_SUBSCRIPTION_TOPIC_ALTITUDE_FUSED, {"Altitude",DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ, nullptr}},
    {DJI_FC_SUBSCRIPTION_TOPIC_ALTITUDE_BAROMETER, {"Altitude AMSL", DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ, nullptr}},
    {DJI_FC_SUBSCRIPTION_TOPIC_GPS_SIGNAL_LEVEL, {"Gps Signal Level", DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, nullptr}},
    {DJI_FC_SUBSCRIPTION_TOPIC_ALTITUDE_OF_HOMEPOINT, {"Home point alt", DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ, nullptr}},
    {DJI_FC_SUBSCRIPTION_TOPIC_GPS_POSITION, {"Gps Position", DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ, nullptr}},
    {DJI_FC_SUBSCRIPTION_TOPIC_CONTROL_DEVICE, {"Control Device", DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ, nullptr}},
    {DJI_FC_SUBSCRIPTION_TOPIC_STATUS_FLIGHT, {"Status flight", DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ, nullptr}},
    {DJI_FC_SUBSCRIPTION_TOPIC_STATUS_DISPLAYMODE, {"Display mode", DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ, nullptr}},
    {DJI_FC_SUBSCRIPTION_TOPIC_RC, {"Rc control", DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ, nullptr}},
    {DJI_FC_SUBSCRIPTION_TOPIC_RC_WITH_FLAG_DATA, {"Rc control", DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ, nullptr}},
    {DJI_FC_SUBSCRIPTION_TOPIC_GPS_DATE, {"Gps Date", DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ, nullptr}},
    {DJI_FC_SUBSCRIPTION_TOPIC_GPS_TIME, {"Gps Time", DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ, nullptr}},
    {DJI_FC_SUBSCRIPTION_TOPIC_GPS_DETAILS, {"Gps details", DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ, nullptr}},
    {DJI_FC_SUBSCRIPTION_TOPIC_RTK_POSITION, {"RTK position", DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ, nullptr}},
    {DJI_FC_SUBSCRIPTION_TOPIC_RTK_POSITION_INFO, {"RTK info", DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ, nullptr}},
};

bool subscribeTopic(const std::string& topicName, E_DjiFcSubscriptionTopic topic,
                    E_DjiDataSubscriptionTopicFreq frequency,
                    DjiReceiveDataOfTopicCallback callback) {
    T_DjiReturnCode djiStat;

    std::cout << "Subscribing " << topicName << " at " << frequency << " Hz." << std::endl;

    djiStat = DjiFcSubscription_SubscribeTopic(topic, frequency, callback);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        std::cerr << "Failed to initialize " << topicName << " subscription " << djiStat << std::endl;
        return false;
    }
    return true;
}

void unsubscibe(const std::string& topicName, const E_DjiFcSubscriptionTopic& topic) {
    if (DjiFcSubscription_UnSubscribeTopic(topic) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        std::cerr << "Failed to unsubscribe topic " << topicName;
    }
}

bool subscribeToTelemetry() {
    bool result = true;
    std::cout << "Starting Telemetry Service..." << std::endl;

    for(auto const& [key, val] : topicSubscriptions)
        if (!subscribeTopic(val.name,
                            key,
                            val.freq,
                            val.callback))
        {
            result = false;
            std::cerr << "Failed to subscribe topic " << val.name << " at " << val.freq << " Hz." << std::endl;
        }

    return result;
}

void unsubscribeFromTelemetry() {
    std::cout << "Closing Telemetry Service..." << std::endl;

    for(auto const& [key, val] : topicSubscriptions)
        unsubscibe(val.name, key);
}

const std::string& getTopicName(E_DjiFcSubscriptionTopic topic)
{
	return topicSubscriptions.at(topic).name;
}
