#include "Formatter.h"

#include <chrono>
#include <sstream>

using namespace std::chrono;

namespace Utils
{

namespace Formatter
{
/**
 * @see Utils::Formatter::formatWithThousand(int,char)
 * @param value
 * @param spaceChar
 * @return
 */
std::string formatWithThousand(int value, char spaceChar /*= ' '*/)
{
	return formatWithThousand(std::to_string(value), spaceChar);
}

/**
 * Simply format such value 1234567 to 1 234 567
 * @param value value which we want format
 * @param spaceChar default space
 * @return formated string
 */
std::string formatWithThousand(std::string value, char spaceChar /*= ' '*/)
{
	std::string spaceCharacter(1, spaceChar);

	for(int i = (int)(value.size()) - 3; i > 0; i -= 3)
	{
		value.insert(i, spaceCharacter);
	}

	return value;
}

std::string formatTime(long long time)
{
	milliseconds myTime(time);

	hours hoursTime = duration_cast<hours>(myTime);
	myTime -= duration_cast<milliseconds>(hoursTime);

	minutes minutesTime = duration_cast<minutes>(myTime);
	myTime -= duration_cast<milliseconds>(minutesTime);

	seconds secondsTime = duration_cast<seconds>(myTime);
	myTime -= duration_cast<milliseconds>(secondsTime);

	char buffer[32];

	if(hoursTime.count() > 0)
	{
		sprintf(buffer,
				"%02d:%02d:%02d:%03d",
				(int)hoursTime.count(),
				(int)minutesTime.count(),
				(int)secondsTime.count(),
				(int)myTime.count());
	}
	else if(minutesTime.count() > 0)
	{
		sprintf(buffer,
				"%02d:%02d:%03d",
				(int)minutesTime.count(),
				(int)secondsTime.count(),
				(int)myTime.count());
	}
	else
	{
		sprintf(buffer, "%02d:%03d", (int)secondsTime.count(), (int)myTime.count());
	}

	return buffer;
}

std::string formatTimeWithoutMilliseconds(std::chrono::seconds time)
{
	hours hoursTime = duration_cast<hours>(time);
	time -= duration_cast<seconds>(hoursTime);

	minutes minutesTime = duration_cast<minutes>(time);
	time -= duration_cast<seconds>(minutesTime);

	seconds secondsTime = duration_cast<seconds>(time);
	time -= secondsTime;

	char buffer[32];

	if(hoursTime.count() > 0)
	{
		sprintf(buffer,
				"%02d:%02d:%02d",
				(int)hoursTime.count(),
				(int)minutesTime.count(),
				(int)secondsTime.count());
	}
	else if(minutesTime.count() > 0)
	{
		sprintf(buffer, "%02d:%02d", (int)minutesTime.count(), (int)secondsTime.count());
	}
	else
	{
		sprintf(buffer, "00:%02d", (int)secondsTime.count());
	}

	return buffer;
}

} // namespace Formatter

} /* namespace Utils */
