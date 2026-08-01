#include "ilic/RepositoryContract.h"

namespace ilic::repository::ports {

std::chrono::system_clock::time_point SystemRepositoryClock::now() const
{
   return std::chrono::system_clock::now();
}

ManualRepositoryClock::ManualRepositoryClock(std::chrono::system_clock::time_point initial)
   : value_(initial)
{
}

std::chrono::system_clock::time_point ManualRepositoryClock::now() const
{
   return value_;
}

void ManualRepositoryClock::advance(std::chrono::milliseconds duration)
{
   value_ += duration;
}

void ManualRepositoryClock::set(std::chrono::system_clock::time_point value)
{
   value_ = value;
}

} // namespace ilic::repository::ports
