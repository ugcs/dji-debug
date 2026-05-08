#include "autopilot_errors.h"
#include <vector>
#include <tuple>
#include <algorithm>
#include <stdexcept>

std::string buildErrorMessage(T_DjiReturnCode code)
{
    static std::vector<std::tuple<T_DjiReturnCode, const char*, const char*>> codes = {DJI_ERROR_OBJECTS};

    auto itr = std::find_if(std::begin(codes),
                            std::end(codes),
                            [code](const std::tuple<T_DjiReturnCode, const char*, const char*>& t)
                                  {return std::get<0>(t) == code;});
    if(itr == codes.end())
        return std::string("Unknown error code ") + std::to_string(code);
    else {
        auto msg = std::get<1>(*itr);
        auto advise = std::get<2>(*itr);
        if(advise != NULL)
          return std::string(msg) + " " + advise + " (Code " + std::to_string(code) + ")";
        else
          return msg;
    }
}

void throwDjiError(T_DjiReturnCode code, const std::string prefix)
{
    throw std::runtime_error(prefix + buildErrorMessage(code));
}
