#ifndef SRC_PING_PONG_INCLUDE_SAMPLE_H_
#define SRC_PING_PONG_INCLUDE_SAMPLE_H_

#include <config.h>

void startApplication(const ParamsCLI& cfg);
void startServices();
void deInitInteface();

void readCameraInfo();
void takePhoto();


#endif /* SRC_PING_PONG_INCLUDE_SAMPLE_H_ */
