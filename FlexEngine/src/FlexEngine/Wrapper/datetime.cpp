#include "datetime.h"


namespace FlexEngine
{

  #pragma region DateTime

  std::tm DateTime::GetDateTime()
  {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm tm{}; localtime_s(&tm, &now);
    return tm;
  }

  std::string DateTime::GetFormattedDateTime(const char* fmt)
  {
    auto time = GetDateTime();

    // convert time to a local time string
    char buffer[256]{ 0 };
    std::strftime(buffer, sizeof(buffer), fmt, &time);
    return buffer;
  }

  #pragma endregion

}