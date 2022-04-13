//CoDWaL - master 9c867f9
#pragma once
#include <string>

class Logger
{
	private:
		static Logger* pInstance;

		bool debugFlag;
		void* pFile;
		void* pMutex;

		Logger(bool forceDebug);

	public:
		static void Init(bool forceDebug = false);
		static void Log(const char* c);
		static void Log(std::wstring c);
		static std::string ToHex(int number);
		static std::istream& GetLine(std::istream& is, std::string& text);

		static inline void Log(std::string c)
		{
			Log(c.c_str());
		}
};
