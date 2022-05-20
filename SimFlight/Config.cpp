#include "Config.h"
#define MINI_CASE_SENSITIVE
#include "mINI/ini.h"

Config::Config()
{
	pData = new mINI::INIStructure();
}

Config::~Config()
{
	delete pData;
}

bool Config::Load()
{
	auto& ini = *(mINI::INIStructure*)pData;
	mINI::INIFile file("config.ini");
	return file.read(ini);
}

std::string Config::GetString(Section section, std::string key)
{
	auto& ini = *(mINI::INIStructure*)pData;
	std::string str;

	GetSectionString(section, str);
	return ini[str][key];
}

int Config::GetInt(Section section, std::string key)
{
	return std::stoi(GetString(section, key));
}

void Config::GetSectionString(Section section, std::string& out)
{
	const char* szOut;

	switch (section)
	{
		case Config::Section::General:
		{
			szOut = "General";
			break;
		}

		case Config::Section::FGConn:
		{
			szOut = "FGConn";
			break;
		}

		case Config::Section::Aircraft:
		{
			szOut = "Aircraft";
			break;
		}

		default:
		{
			szOut = "";
			break;
		}
	}

	out = szOut;
}
