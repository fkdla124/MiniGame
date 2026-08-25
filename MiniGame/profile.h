#pragma once

#include <cstdint>
#include <windows.h>

void ProfileBegin(const char* name);
void ProfileEnd(const char* name);

void ProfileDataOutText(const char* fileName);

void ProfileReset(void);

double GetRdtscFrequency(void);

double CalibrateRdtscFrequency(void);

typedef struct PROFILE_DATA
{
	char name[64];

	int64_t startTime;
	
	int64_t totalTime;
	int64_t min[2];
	int64_t max[2];

	int64_t call;
} PROFILE_DATA;

#define PROFILES_SIZE 50
extern PROFILE_DATA profiles[PROFILES_SIZE];




#ifdef PROFILE

class Profile
{
private:
	const char* _name;

public:
	Profile(const char* name) : _name(name)
	{
		ProfileBegin(_name);
	}
	~Profile()
	{
		ProfileEnd(_name);
	}
};

	#define PROFILE_CONCAT_IMPL(x, y) x##y
	#define PROFILE_CONCAT(x, y) PROFILE_CONCAT_IMPL(x, y)
	#define PROFILE_SCOPE(name) Profile PROFILE_CONCAT(profile_, __COUNTER__)(name)

	#define PROFILE_BEGIN(TagName)	ProfileBegin(TagName)
	#define PROFILE_END(TagName)	ProfileEnd(TagName)

#else

	#define PROFILE_SCOPE(name)
	#define PROFILE_BEGIN(TagName)
	#define PROFILE_END(TagName)

#endif