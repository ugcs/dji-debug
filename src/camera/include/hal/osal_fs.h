#ifndef OSAL_FS_H
#define OSAL_FS_H

#include "dji_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

T_DjiReturnCode Osal_FileOpen(const char *fileName, const char *fileMode, T_DjiFileHandle *fileObj);
T_DjiReturnCode Osal_FileClose(T_DjiFileHandle fileObj);
T_DjiReturnCode Osal_FileWrite(T_DjiFileHandle fileObj, const uint8_t *buf, uint32_t len, uint32_t *realLen);
T_DjiReturnCode Osal_FileRead(T_DjiFileHandle fileObj, uint8_t *buf, uint32_t len, uint32_t *realLen);
T_DjiReturnCode Osal_FileSeek(T_DjiFileHandle fileObj, uint32_t offset);
T_DjiReturnCode Osal_FileSync(T_DjiFileHandle fileObj);
T_DjiReturnCode Osal_DirOpen(const char *filePath, T_DjiDirHandle *dirObj);
T_DjiReturnCode Osal_DirClose(T_DjiDirHandle dirObj);
T_DjiReturnCode Osal_DirRead(T_DjiDirHandle dirObj, T_DjiFileInfo *fileInfo);
T_DjiReturnCode Osal_Mkdir(const char *filePath);
T_DjiReturnCode Osal_Unlink(const char *filePath);
T_DjiReturnCode Osal_Rename(const char *oldFilePath, const char *newFilePath);
T_DjiReturnCode Osal_Stat(const char *filePath, T_DjiFileInfo *fileInfo);

#ifdef __cplusplus
}
#endif

#endif // OSAL_FS_H
