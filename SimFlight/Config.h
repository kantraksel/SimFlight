#pragma once
#include <string>

class Config
{
	public:
		enum class Section
		{
			General,
			FGConn,
			Aircraft,
		};

	private:
		void* pData;

		void GetSectionString(Section section, std::string& out);

	public:
		Config();
		~Config();

		bool Load();

		std::string GetString(Section section, std::string key);
		int GetInt(Section section, std::string key);
};
