#pragma once
#include <dji_typedef.h>
#include <dji_fc_subscription.h>
#include <vector>
#include <map>
#include <string>

typedef  struct {
    const std::string name;
    const E_DjiDataSubscriptionTopicFreq freq;
    const DjiReceiveDataOfTopicCallback callback;
} TopicSubsction;

bool subscribeTopic(const std::string& topicName, E_DjiFcSubscriptionTopic topic,
                    E_DjiDataSubscriptionTopicFreq frequency,
                    DjiReceiveDataOfTopicCallback callback  = nullptr);


T_DjiReturnCode onMobileDataReceived(const uint8_t *data, uint16_t len);
void unsubscibe(const std::string& topicName, const E_DjiFcSubscriptionTopic& topic);
bool subscribeToTelemetry();
void unsubscribeFromTelemetry();

const std::string& getTopicName(E_DjiFcSubscriptionTopic topic);


