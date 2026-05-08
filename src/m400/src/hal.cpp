#include <hal.h>
#include <autopilot_errors.h>
#include <dji_logger.h>
#include <exception>

void setupEnvironment() {

    T_DjiReturnCode returnCode;
    T_DjiOsalHandler osalHandler = {0};
    T_DjiHalUsbBulkHandler usbBulkHandler = {0};
    T_DjiLoggerConsole printConsole;
    T_DjiFileSystemHandler fileSystemHandler = {0};
    T_DjiSocketHandler socketHandler = {0};
    T_DjiHalNetworkHandler networkHandler = {0};

    socketHandler.Socket = Osal_Socket;
    socketHandler.Bind = Osal_Bind;
    socketHandler.Close = Osal_Close;
    socketHandler.UdpSendData = Osal_UdpSendData;
    socketHandler.UdpRecvData = Osal_UdpRecvData;
    socketHandler.TcpListen = Osal_TcpListen;
    socketHandler.TcpAccept = Osal_TcpAccept;
    socketHandler.TcpConnect = Osal_TcpConnect;
    socketHandler.TcpSendData = Osal_TcpSendData;
    socketHandler.TcpRecvData = Osal_TcpRecvData;

    osalHandler.TaskCreate = Osal_TaskCreate;
    osalHandler.TaskDestroy = Osal_TaskDestroy;
    osalHandler.TaskSleepMs = Osal_TaskSleepMs;
    osalHandler.MutexCreate = Osal_MutexCreate;
    osalHandler.MutexDestroy = Osal_MutexDestroy;
    osalHandler.MutexLock = Osal_MutexLock;
    osalHandler.MutexUnlock = Osal_MutexUnlock;
    osalHandler.SemaphoreCreate = Osal_SemaphoreCreate;
    osalHandler.SemaphoreDestroy = Osal_SemaphoreDestroy;
    osalHandler.SemaphoreWait = Osal_SemaphoreWait;
    osalHandler.SemaphoreTimedWait = Osal_SemaphoreTimedWait;
    osalHandler.SemaphorePost = Osal_SemaphorePost;
    osalHandler.Malloc = Osal_Malloc;
    osalHandler.Free = Osal_Free;
    osalHandler.GetTimeMs = Osal_GetTimeMs;
    osalHandler.GetTimeUs = Osal_GetTimeUs;
    osalHandler.GetRandomNum = Osal_GetRandomNum,

    printConsole.func = DjiLogging_PrintConsole;
    printConsole.consoleLevel = DJI_LOGGER_CONSOLE_LOG_LEVEL_INFO;
    printConsole.isSupportColor = true;

    usbBulkHandler.UsbBulkInit = HalUsbBulk_Init;
    usbBulkHandler.UsbBulkDeInit = HalUsbBulk_DeInit;
    usbBulkHandler.UsbBulkWriteData = HalUsbBulk_WriteData;
    usbBulkHandler.UsbBulkReadData = HalUsbBulk_ReadData;
    usbBulkHandler.UsbBulkGetDeviceInfo = HalUsbBulk_GetDeviceInfo;

    fileSystemHandler.FileOpen = Osal_FileOpen,
    fileSystemHandler.FileClose = Osal_FileClose,
    fileSystemHandler.FileWrite = Osal_FileWrite,
    fileSystemHandler.FileRead = Osal_FileRead,
    fileSystemHandler.FileSync = Osal_FileSync,
    fileSystemHandler.FileSeek = Osal_FileSeek,
    fileSystemHandler.DirOpen = Osal_DirOpen,
    fileSystemHandler.DirClose = Osal_DirClose,
    fileSystemHandler.DirRead = Osal_DirRead,
    fileSystemHandler.Mkdir = Osal_Mkdir,
    fileSystemHandler.Unlink = Osal_Unlink,
    fileSystemHandler.Rename = Osal_Rename,
    fileSystemHandler.Stat = Osal_Stat,

    returnCode = DjiPlatform_RegOsalHandler(&osalHandler);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
    	throwDjiError(returnCode,"Register osal handler error.");
    }

    returnCode = DjiPlatform_RegHalUsbBulkHandler(&usbBulkHandler);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
    	throwDjiError(returnCode,"Register osal handler error.");
    }

    returnCode = DjiPlatform_RegSocketHandler(&socketHandler);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
    	throwDjiError(returnCode,"Register osal handler error.");
    }

    returnCode = DjiLogger_AddConsole(&printConsole);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
    	throwDjiError(returnCode,"Register osal handler error.");
    }
}
