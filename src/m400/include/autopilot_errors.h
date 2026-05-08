#pragma once

#include <dji_typedef.h>
#include <string>

std::string buildErrorMessage(T_DjiReturnCode code);

void throwDjiError(T_DjiReturnCode code, const std::string prefix = "");
