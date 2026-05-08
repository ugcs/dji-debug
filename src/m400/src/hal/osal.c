#define _GNU_SOURCE
#include <hal/osal.h>
#include "dji_typedef.h"

#include <stdlib.h>

#define UNUSED(x) (void)(x)

static uint32_t s_localTimeMsOffset = 0;
static uint64_t s_localTimeUsOffset = 0;

T_DjiReturnCode Osal_TaskCreate(const char *name, void *(*taskFunc)(void *), uint32_t stackSize, void *arg,
                                T_DjiTaskHandle *task)
{
    UNUSED(stackSize);

    int result;
    char nameDealed[16] = {0};

    *task = malloc(sizeof(pthread_t));
    if (*task == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    result = pthread_create(*task, NULL, taskFunc, arg);
    if (result != 0) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    if (name != NULL) {
        strncpy(nameDealed, name, sizeof(nameDealed) - 1);
    }
    result = pthread_setname_np(*(pthread_t *) *task, nameDealed);
    if (result != 0) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode Osal_TaskDestroy(T_DjiTaskHandle task)
{
    pthread_cancel(*(pthread_t *) task);

    free(task);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode Osal_TaskSleepMs(uint32_t timeMs)
{
    usleep(1000 * timeMs);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * @brief Declare the mutex container, initialize the mutex, and
 * create mutex ID.
 * @param mutex:  pointer to the created mutex ID.
 * @return an enum that represents a status of PSDK
 */
T_DjiReturnCode Osal_MutexCreate(T_DjiMutexHandle *mutex)
{
    int result;

    *mutex = malloc(sizeof(pthread_mutex_t));
    if (*mutex == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    result = pthread_mutex_init(*mutex, NULL);
    if (result != 0) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * @brief Delete the created mutex.
 * @param mutex:  pointer to the created mutex ID.
 * @return an enum that represents a status of PSDK
 */
T_DjiReturnCode Osal_MutexDestroy(T_DjiMutexHandle mutex)
{
    int result;

    result = pthread_mutex_destroy(mutex);
    if (result != 0) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }
    free(mutex);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * @brief Acquire and lock the mutex when peripheral access is required
 * @param mutex:  pointer to the created mutex ID.
 * @return an enum that represents a status of PSDK
 */
T_DjiReturnCode Osal_MutexLock(T_DjiMutexHandle mutex)
{
    if (!mutex)
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;

    int result = pthread_mutex_lock(mutex);

    if (result != 0) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * @brief Unlock and release the mutex, when done with the peripheral access.
 * @param mutex:  pointer to the created mutex ID.
 * @return an enum that represents a status of PSDK
 */
T_DjiReturnCode Osal_MutexUnlock(T_DjiMutexHandle mutex)
{
    if (!mutex)
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;

    int result = pthread_mutex_unlock(mutex);

    if (result != 0) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * @brief Declare the semaphore container, initialize the semaphore, and
 * create semaphore ID.
 * @param semaphore: pointer to the created semaphore ID.
 * @param initValue: initial value of semaphore.
 * @return an enum that represents a status of PSDK
 */
T_DjiReturnCode Osal_SemaphoreCreate(uint32_t initValue, T_DjiSemaHandle *semaphore)
{
    int result;

    *semaphore = malloc(sizeof(sem_t));
    if (*semaphore == NULL) {
        return
            DJI_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    result = sem_init(*semaphore, 0, (unsigned int) initValue);
    if (result != 0) {
        return
            DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return
        DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * @brief Delete the created semaphore.
 * @param semaphore: pointer to the created semaphore ID.
 * @return an enum that represents a status of PSDK
 */
T_DjiReturnCode Osal_SemaphoreDestroy(T_DjiSemaHandle semaphore)
{
    if (!semaphore) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    int result;

    result = sem_destroy((sem_t *) semaphore);
    if (result != 0) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    free(semaphore);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * @brief Wait the semaphore until token becomes available.
 * @param semaphore: pointer to the created semaphore ID.
 * @return an enum that represents a status of PSDK
 */
T_DjiReturnCode Osal_SemaphoreWait(T_DjiSemaHandle semaphore)
{
    if (!semaphore) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    int result;

    result = sem_wait(semaphore);
    if (result != 0) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * @brief Wait the semaphore until token becomes available.
 * @param semaphore: pointer to the created semaphore ID.
 * @param waitTime: timeout value of waiting semaphore, unit: millisecond.
 * @return an enum that represents a status of PSDK
 */
T_DjiReturnCode Osal_SemaphoreTimedWait(T_DjiSemaHandle semaphore, uint32_t waitTime)
{
    if (!semaphore) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    int result;
    struct timespec semaphoreWaitTime;

    clock_gettime(CLOCK_MONOTONIC, &semaphoreWaitTime);
    semaphoreWaitTime.tv_nsec += waitTime * 1000000;
    if (semaphoreWaitTime.tv_nsec >= 1000000000) {
        semaphoreWaitTime.tv_sec += semaphoreWaitTime.tv_nsec / 1000000000;
        semaphoreWaitTime.tv_nsec %= 1000000000;
    }


    result = sem_timedwait(semaphore, &semaphoreWaitTime);
    if (result != 0) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * @brief Release the semaphore token.
 * @param semaphore: pointer to the created semaphore ID.
 * @return an enum that represents a status of PSDK
 */
T_DjiReturnCode Osal_SemaphorePost(T_DjiSemaHandle semaphore)
{
    if (!semaphore) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    int result;

    result = sem_post(semaphore);
    if (result != 0) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * @brief Get the system time for ms.
 * @return an uint32 that the time of system, uint:ms
 */
T_DjiReturnCode Osal_GetTimeMs(uint32_t *ms)
{
    struct timespec semaphoreWaitTime;
    clock_gettime(CLOCK_MONOTONIC, &semaphoreWaitTime);
    *ms = (semaphoreWaitTime.tv_sec * 1000 + semaphoreWaitTime.tv_nsec / 1000000);
    if (s_localTimeMsOffset == 0) {
        s_localTimeMsOffset = *ms;
    } else {        
        *ms = *ms - s_localTimeMsOffset;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode Osal_GetTimeUs(uint64_t *us)
{
    struct timespec semaphoreWaitTime;
    clock_gettime(CLOCK_MONOTONIC, &semaphoreWaitTime);
    *us = (semaphoreWaitTime.tv_sec * 1000000 + semaphoreWaitTime.tv_nsec / 1000);
    if (s_localTimeUsOffset == 0) {
        s_localTimeUsOffset = *us;
    } else {        
        *us = *us - s_localTimeUsOffset;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

void *Osal_Malloc(uint32_t size)
{
    return malloc(size);
}

void Osal_Free(void *ptr)
{
    free(ptr);
}


T_DjiReturnCode Osal_GetRandomNum(uint16_t *randomNum)
{
    srand(time(NULL));
    *randomNum = random() % 65535;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}
