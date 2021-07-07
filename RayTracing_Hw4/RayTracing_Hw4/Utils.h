#ifndef UTILS_H_
#define UTILS_H_

#include <chrono>
#include <iostream>

template <typename T>
void printTimeDuration(T start, T end)
{
	auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

	auto const usecs = diff % 1000;
	diff /= 1000;
	auto const msecs = diff % 1000;
	diff /= 1000;
	auto const secs = diff % 60;
	diff /= 60;
	auto const mins = diff % 60;
	diff /= 60;
	auto const hours = diff % 24;
	diff /= 24;
	auto const days = diff;
	
	std::cout << "Image is rendered in ";

	bool printedEarlier = false;
	if (days >= 1) {
		printedEarlier = true;
		std::cout << days << (1 != days ? " days" : " day") << ' ';
	}

	if (printedEarlier || hours >= 1) {
		printedEarlier = true;
		std::cout << hours << (1 != hours ? " hours" : " hour") << ' ';
	}

	if (printedEarlier || mins >= 1) {
		printedEarlier = true;
		std::cout << mins << (1 != mins ? " mins" : " min") << ' ';
	}

	if (printedEarlier || secs >= 1) {
		printedEarlier = true;
		std::cout << secs << (1 != secs ? " secs" : " sec") << ' ';
	}

	if (printedEarlier || msecs >= 1) {
		printedEarlier = true;
		std::cout << msecs << (1 != msecs ? " msecs" : " msec") << ' ';
	}

	if (printedEarlier || usecs >= 1) {
		printedEarlier = true;
		std::cout << usecs << (1 != usecs ? " usecs" : " usec");
	}
}

#endif