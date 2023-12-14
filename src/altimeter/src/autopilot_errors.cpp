#include "autopilot_errors.h"
#include <vector>
#include <tuple>
#include <algorithm>
#include <stdexcept>
#include <iomanip>

std::string int_to_hex(unsigned int val)
{
	std::stringstream stream;
	stream << "0x" << std::hex << val;
	return stream.str();
}

std::string buildErrorMessage(T_DjiReturnCode code)
{
    static std::vector<std::tuple<T_DjiReturnCode, const char*, const char*>> codes = {DJI_ERROR_OBJECTS};

    auto itr = std::find_if(std::begin(codes),
                            std::end(codes),
                            [code](const std::tuple<T_DjiReturnCode, const char*, const char*>& t)
                                  {return std::get<0>(t) == code;});
    if(itr == codes.end())
        return std::string("Unknown error code ") + int_to_hex(code);
    else {
        auto msg = std::get<1>(*itr);
        auto advise = std::get<2>(*itr);
        if(advise != NULL)
          return std::string(msg) + " " + advise + " (Code " + int_to_hex(code) + ")";
        else
          return msg;
    }
}

void throwDjiError(T_DjiReturnCode code, const std::string prefix)
{
    throw std::runtime_error(prefix + buildErrorMessage(code));
}
