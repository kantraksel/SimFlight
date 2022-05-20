#pragma once
#include "LocalAirplane.h"
#include "Config.h"

class App
{
	private:
		SCClient client;
		LocalAirplane airplane;
		bool isAppRunning;
		Config config;

		void SimConnectThread();
		void AirplaneThread();

		void InitEnv();
		void PrepareClient();
		void PrepareAirplane();

	public:
		void Main();
};
