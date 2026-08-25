#include "profile.h"
#include <cstdio>
#include <thread>
#include <chrono>

#pragma comment(lib, "winmm.lib")

PROFILE_DATA profiles[PROFILES_SIZE];

void ProfileBegin(const char* name)
{
	int createIndex = -1;
	for (int i = 0; i < PROFILES_SIZE; i++)
	{
		if (strcmp(profiles[i].name, name) == 0)
		{
			if (profiles[i].startTime != 0)
			{
				// double Begin Error
				__debugbreak();
			}

			profiles[i].startTime = __rdtsc();
			return;
		}

		if (profiles[i].name[0] == '\0' && createIndex == -1)
		{
			createIndex = i;
		}
	}

	if (createIndex == -1)
	{
		// profiles Array Full. error
		__debugbreak();
	}

	strcpy_s(profiles[createIndex].name, name);
	profiles[createIndex].startTime = __rdtsc();
}
void ProfileEnd(const char* name)
{
	for (int i = 0; i < PROFILES_SIZE; i++)
	{
		if (strcmp(profiles[i].name, name) == 0)
		{
			int64_t elapsed;
			elapsed = __rdtsc();
			elapsed -= profiles[i].startTime;

			profiles[i].totalTime += elapsed;
			profiles[i].call += 1;
			profiles[i].startTime = 0;

			if (elapsed < profiles[i].min[0] || profiles[i].min[0] == 0)
			{
				profiles[i].min[1] = profiles[i].min[0];
				profiles[i].min[0] = elapsed;
			}
			else if (elapsed < profiles[i].min[1] || profiles[i].min[1] == 0)
			{
				profiles[i].min[1] = elapsed;
			}

			if (elapsed > profiles[i].max[0] || profiles[i].max[0] == 0)
			{
				profiles[i].max[1] = profiles[i].max[0];
				profiles[i].max[0] = elapsed;
			}
			else if (elapsed > profiles[i].max[1] || profiles[i].max[1] == 0)
			{
				profiles[i].max[1] = elapsed;
			}

			return;
		}
	}
}

void ProfileReset(void)
{
	for (int i = 0; i < PROFILES_SIZE; i++)
	{
		profiles[i].startTime = 0;
		profiles[i].totalTime = 0;
		profiles[i].min[0] = 0;
		profiles[i].min[1] = 0;
		profiles[i].max[0] = 0;
		profiles[i].max[1] = 0;
		profiles[i].call = 0;
	}
}

double GetRdtscFrequency(void)
{
	static const double frequency = CalibrateRdtscFrequency();
	return frequency;

}

double CalibrateRdtscFrequency(void)
{
	LARGE_INTEGER freq;
	LARGE_INTEGER qpcStart, qpcEnd;
	int64_t rdtscStart, rdtscEnd;
	UINT unused;

	timeBeginPeriod(1);
	QueryPerformanceFrequency(&freq);

	QueryPerformanceCounter(&qpcStart);

	_mm_lfence();
	rdtscStart = __rdtsc();

	std::this_thread::sleep_for(std::chrono::milliseconds(200));

	rdtscEnd = __rdtscp(&unused);
	_mm_lfence();

	QueryPerformanceCounter(&qpcEnd);

	double qpcElapsed = static_cast<double>(qpcEnd.QuadPart - qpcStart.QuadPart) / freq.QuadPart;
	double rdtscElapsed = static_cast<double>(rdtscEnd - rdtscStart);

	return (rdtscElapsed / qpcElapsed);
}


void ProfileDataOutText(const char* fileName)
{
	HANDLE hFile = CreateFileA(fileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		__debugbreak();

	char buffer[8192];
	int offset = 0;

	const int nameColumnWidth = 63;
	const int otherColumnWidth = 20;

	offset += sprintf_s(
		buffer + offset,
		sizeof(buffer) - offset,
		"%*s|%*s|%*s|%*s|%*s|\n", nameColumnWidth - 1, "Name     ", otherColumnWidth - 1, "Average     ", otherColumnWidth - 1, "Min      ", otherColumnWidth - 1, "Max     ", otherColumnWidth - 1, "Call     ");

	char line[nameColumnWidth + otherColumnWidth * 4 + 2];

	memset(line, '-', sizeof(line) - 2);
	line[sizeof(line) - 2] = '\n';
	line[sizeof(line) - 1] = '\0';

	offset += sprintf_s(
		buffer + offset,
		sizeof(buffer) - offset,
		"%s",
		line);

	int save = offset;

	double freq = GetRdtscFrequency();

	for (int i = 0; i < PROFILES_SIZE; i++)
	{
		if (profiles[i].name[0] == '\0')
			continue;

		double average = static_cast<double>(profiles[i].totalTime);

		if (profiles[i].call > 4)
		{
			average -= profiles[i].min[0];
			average -= profiles[i].min[1];
			average -= profiles[i].max[0];
			average -= profiles[i].max[1];
			average /= profiles[i].call - 4;
		}
		else
		{
			average /= profiles[i].call;
		}

		average /= (freq / 1000000);

		double min = static_cast<double>(profiles[i].min[0]) / (freq / 1000000);
		double max = static_cast<double>(profiles[i].max[0]) / (freq / 1000000);

		offset += sprintf_s(
			buffer + offset,
			sizeof(buffer) - offset,
			"%*s|%*.*f us|%*.*f us|%*.*f us|%*lld|\n",
			nameColumnWidth - 1, profiles[i].name,
			otherColumnWidth - 4, 4, average,
			otherColumnWidth - 4, 4, min,
			otherColumnWidth - 4, 4, max,
			otherColumnWidth - 1, profiles[i].call);
	}

	if (offset == save)
	{
		offset += sprintf_s(
			buffer + offset,
			sizeof(buffer) - offset,
			"No Profiling Data....\n");
	}

	offset += sprintf_s(
		buffer + offset,
		sizeof(buffer) - offset,
		"%s",
		line);

	DWORD bytesWritten;
	if (!WriteFile(hFile, buffer, offset, &bytesWritten, NULL))
	{
		CloseHandle(hFile);
		__debugbreak();
	}

	CloseHandle(hFile);
}