#include <dji_logger.h>
#include <hal/hal_uart.h>

#define UART_DEV_NAME_STR_SIZE             (128)
#define DJI_SYSTEM_CMD_STR_MAX_SIZE        (64)
#define DJI_SYSTEM_RESULT_STR_MAX_SIZE     (128)

typedef struct {
    int uartFd;
} T_UartHandleStruct;


char uartDevice_0[UART_DEV_NAME_STR_SIZE];
char uartDevice_1[UART_DEV_NAME_STR_SIZE];

bool useSerial = false;
bool useUsbDevice = false;

void HalUart_SetSerial(const char* dev, E_DjiHalUartNum num)
{
    if (num == DJI_HAL_UART_NUM_0) {
        strcpy(uartDevice_0, dev);
        useSerial = true;
    } else if (num == DJI_HAL_UART_NUM_1) {
        strcpy(uartDevice_1, dev);
        useUsbDevice = true;
    }
}


T_DjiReturnCode HalUart_Init(E_DjiHalUartNum uartNum, uint32_t baudRate, T_DjiUartHandle *uartHandle)
{
    T_UartHandleStruct *uartHandleStruct;
    struct termios options;
    struct flock lock;
    T_DjiReturnCode returnCode = DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    char uartName[UART_DEV_NAME_STR_SIZE];

    FILE *fp = NULL;

    uartHandleStruct = malloc(sizeof(T_UartHandleStruct));
    if (uartHandleStruct == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    if (uartNum == DJI_HAL_UART_NUM_0) {
        strcpy(uartName, uartDevice_0);
    } else if (uartNum == DJI_HAL_UART_NUM_1) {
        strcpy(uartName, uartDevice_1);
    } else {
        goto free_uart_handle;
    }

    uartHandleStruct->uartFd = open(uartName, (unsigned) O_RDWR | (unsigned) O_NOCTTY | (unsigned) O_NDELAY);
    if (uartHandleStruct->uartFd == -1) {
        goto close_fp;
    }

    // Forbid multiple psdk programs to access the serial port
    lock.l_type = F_WRLCK;
    lock.l_pid = getpid();
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    if (fcntl(uartHandleStruct->uartFd, F_GETLK, &lock) < 0) {
        goto close_uart_fd;
    }
    if (lock.l_type != F_UNLCK) {
        goto close_uart_fd;
    }
    lock.l_type = F_WRLCK;
    lock.l_pid = getpid();
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    if (fcntl(uartHandleStruct->uartFd, F_SETLKW, &lock) < 0) {
        goto close_uart_fd;
    }

    if (tcgetattr(uartHandleStruct->uartFd, &options) != 0) {
        goto close_uart_fd;
    }

    switch (baudRate) {
        case 115200:
            cfsetispeed(&options, B115200);
            cfsetospeed(&options, B115200);
            break;
        case 230400:
            cfsetispeed(&options, B230400);
            cfsetospeed(&options, B230400);
            break;
        case 460800:
            cfsetispeed(&options, B460800);
            cfsetospeed(&options, B460800);
            break;
        case 921600:
            cfsetispeed(&options, B921600);
            cfsetospeed(&options, B921600);
            break;
        case 1000000:
            cfsetispeed(&options, B1000000);
            cfsetospeed(&options, B1000000);
            break;
        default:
            goto close_uart_fd;
    }

    options.c_cflag |= (unsigned) CLOCAL;
    options.c_cflag |= (unsigned) CREAD;
    options.c_cflag &= ~(unsigned) CRTSCTS;
    options.c_cflag &= ~(unsigned) CSIZE;
    options.c_cflag |= (unsigned) CS8;
    options.c_cflag &= ~(unsigned) PARENB;
    options.c_iflag &= ~(unsigned) INPCK;
    options.c_cflag &= ~(unsigned) CSTOPB;
    options.c_oflag &= ~(unsigned) OPOST;
    options.c_lflag &= ~((unsigned) ICANON | (unsigned) ECHO | (unsigned) ECHOE | (unsigned) ISIG);
    options.c_iflag &= ~((unsigned) BRKINT | (unsigned) ICRNL | (unsigned) INPCK | (unsigned) ISTRIP | (unsigned) IXON);
    options.c_cc[VTIME] = 0;
    options.c_cc[VMIN] = 0;

    tcflush(uartHandleStruct->uartFd, TCIFLUSH);

    if (tcsetattr(uartHandleStruct->uartFd, TCSANOW, &options) != 0) {
        goto close_uart_fd;
    }

    *uartHandle = uartHandleStruct;
    if (fp)
        pclose(fp);

    return returnCode;

close_uart_fd:
    close(uartHandleStruct->uartFd);

close_fp:
    if(fp)
      pclose(fp);

free_uart_handle:
    free(uartHandleStruct);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
}

T_DjiReturnCode HalUart_DeInit(T_DjiUartHandle uartHandle)
{
    int32_t ret;
    T_UartHandleStruct *uartHandleStruct = (T_UartHandleStruct *) uartHandle;

    if (uartHandle == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    ret = close(uartHandleStruct->uartFd);
    free(uartHandleStruct);
    if (ret < 0)
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalUart_WriteData(T_DjiUartHandle uartHandle, const uint8_t *buf, uint32_t len, uint32_t *realLen)
{
    int32_t ret;
    T_UartHandleStruct *uartHandleStruct = (T_UartHandleStruct *) uartHandle;

    if (uartHandle == NULL || buf == NULL || len == 0 || realLen == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    ret = write(uartHandleStruct->uartFd, buf, len);
    if (ret >= 0) {
        *realLen = ret;
    } else {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalUart_ReadData(T_DjiUartHandle uartHandle, uint8_t *buf, uint32_t len, uint32_t *realLen)
{
    int32_t ret;
    T_UartHandleStruct *uartHandleStruct = (T_UartHandleStruct *) uartHandle;

    if (uartHandle == NULL || buf == NULL || len == 0 || realLen == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    ret = read(uartHandleStruct->uartFd, buf, len);
    if (ret >= 0) {
        *realLen = ret;
    } else {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalUart_GetStatus(E_DjiHalUartNum uartNum, T_DjiUartStatus *status)
{
    if (uartNum == DJI_HAL_UART_NUM_0) {
        status->isConnect = useSerial;
    } else if (uartNum == DJI_HAL_UART_NUM_1) {
        status->isConnect = useUsbDevice;
    } else {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}
