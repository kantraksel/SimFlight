//CoDWaL - master 9c867f9
#include "Logger.h"
#include <chrono>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <shared_mutex>
#include <sstream>
#ifdef _WINDOWS
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
#endif

#ifdef _DEBUG
#define DEBUG_FLAG true
#else
#define DEBUG_FLAG false
#endif

Logger* Logger::pInstance = nullptr;

void Logger::Init(bool forceDebug)
{
	pInstance = new Logger(forceDebug);
	Log("Initialized logger!");
	if (pInstance->debugFlag)
		Log("Debug flag is set!");
}

void Logger::Log(const char* c)
{
	auto& mutex = *((std::shared_mutex*)pInstance->pMutex);

	mutex.lock();

	time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	tm buff;
	localtime_s(&buff, &time);
	auto now = std::put_time(&buff, "%T");

	std::cout << '[' << now << ']' << ' ' << c << std::endl;
	if (pInstance->pFile)
	{
		auto& str = *((std::ofstream*)pInstance->pFile);
		str << '[' << now << ']' << ' ' << c << std::endl;
		str.flush();
	}
	
	mutex.unlock();
}

void Logger::Log(std::wstring c)
{
	std::string str;
	str.reserve(c.size());
	for (wchar_t ch : c)
	{
		str.push_back(ch);
	}
	Log(str);
}

Logger::Logger(bool forceDebug)
{
	debugFlag = forceDebug || DEBUG_FLAG || std::filesystem::exists("debug.txt");
#ifdef _WINDOWS
	if (debugFlag && AllocConsole())
	{
		FILE* fDummy;
		freopen_s(&fDummy, "CONIN$", "r", stdin);
		freopen_s(&fDummy, "CONOUT$", "w", stderr);
		freopen_s(&fDummy, "CONOUT$", "w", stdout);
	}
#endif
	auto stream = new std::ofstream("log.txt");
	if (!stream)
	{
		//std::abort();
		delete stream;
		stream = nullptr;
	}
	pFile = stream;
	pMutex = new std::shared_mutex();
}

std::istream& Logger::GetLine(std::istream& is, std::string& text)
{
	return std::getline(is, text);
}

std::string Logger::ToHex(int number)
{
	std::stringstream str;
	str << std::setfill('0') << std::setw(sizeof(int) * 2) << std::hex << number;
	return str.str();
}
