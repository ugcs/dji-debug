#ifndef HAL_LOG_H
#define HAL_LOG_H

#include <dji_typedef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*writer_cb_t)(const uint8_t *data, uint16_t dataLen);

T_DjiReturnCode DjiLogging_Init();
T_DjiReturnCode DjiLogging_PrintConsole(const uint8_t *data, uint16_t dataLen);

#ifdef __cplusplus
}
#endif

#endif // HAL_LOG_H
