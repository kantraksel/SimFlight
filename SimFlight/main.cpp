#include "App.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' " \
	"name='" "Microsoft.FlightSimulator.SimConnect" "' " \
	"version='" "10.0.63003.0" "' " \
	"processorArchitecture='x86' " \
	"publicKeyToken='" "7554e4e7414ac468" "'\"")

int main()
{
	App app;
	app.Main();
	return 0;
}
