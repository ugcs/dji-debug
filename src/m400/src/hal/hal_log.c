#include <hal/hal_log.h>
#include <dji_logger.h>
#include <unistd.h>
#include <errno.h>

#include <stdio.h>
#include <time.h>


FILE *s_djiLogFile;
FILE *s_djiLogFileCnt;

writer_cb_t externalConsoleWriter = NULL;

T_DjiReturnCode DjiLogging_Init() {
    T_DjiLoggerConsole printConsole = {
        .func = DjiLogging_PrintConsole,
        .consoleLevel = DJI_LOGGER_CONSOLE_LOG_LEVEL_INFO,
        .isSupportColor = true,
    };

    T_DjiReturnCode returnCode;

    returnCode = DjiLogger_AddConsole(&printConsole);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        printf("Add printf console error.");
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiLogging_PrintConsole(const uint8_t *data, uint16_t len)
{
    printf("%s", data);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}
