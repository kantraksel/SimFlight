#include <chrono>
#include <thread>
#include "Logger.h"
#include "SocketApi.h"
#include "FGServer.h"
#include "SCClient.h"

#define APP_VERSION "0.1.0"
//NOTE: this data is temporarily constant - will be loaded from file in future
#define FG_SERVER "217.78.131.42" //mpserver01.flightgear.org - highest availability
#define FG_PORT 5000
#define CALLSIGN "callsig" //default from FG
#define AIRCRAFT_PATH "Aircraft/c172p/Models/c172p.xml" //default from FG

class App
{
	private:
		FGServer fg;
		SCClient sc;
		SCClient::AirplaneData airplaneData;
		bool isFGRunning;

		void SimConnectThread();
		void FlightGearThread();

		void SCUpdateData(SCClient::AirplaneData& data);
		double FGGetTime();

	public:
		void Main();
};

int main()
{
	App app;
	app.Main();
	return 0;
}

void App::Main()
{
	Logger::Init();
	Logger::Log("SimFlight " APP_VERSION);

	Logger::Log("Initializing...");
	SocketApi::Init();
	sc.SetUpdateDataEvent([&](SCClient::AirplaneData& data) { SCUpdateData(data); });
	if (!sc.Connect())
	{
		Logger::Log("Failed to connect to Flight Simulator!");
		return;
	}
	Logger::Log("Connected to Flight Simulator");

	fg.SelectServer(FG_SERVER, FG_PORT);
	fg.SetPeer(CALLSIGN, AIRCRAFT_PATH);
	Logger::Log("Prepared FlightGear connection");

	isFGRunning = true;
	std::thread scThread(&App::SimConnectThread, this);
	std::thread fgThread(&App::FlightGearThread, this);

	Logger::Log("Initialized! Flight has been started");

	Logger::Log("Press any key to exit");
	getchar();

	Logger::Log("Shutting down");
	isFGRunning = false;
	sc.Disconnnect();
	fgThread.join();
	Logger::Log("FlightGear thread ended");
	scThread.join();
	Logger::Log("SimConnect thread ended");

	SocketApi::Shutdown();
}

void App::SimConnectThread()
{
	sc.RunLoop();
	isFGRunning = false;
}

void App::FlightGearThread()
{
#define NOW_MILLI (std::chrono::steady_clock::now().time_since_epoch().count() * NanoToMilli)
	constexpr double NanoToMilli = 0.000001;
	constexpr double MilliToCore = 0.0001;
	constexpr long long FGSendRateMs = 100;

	FGServer::AirplaneData data;

	while (isFGRunning)
	{
		double begin = NOW_MILLI;

		data.Longitude = airplaneData.longitude;
		data.Latitude = airplaneData.latitude;
		data.Altitude = airplaneData.altitude;
		data.Heading = airplaneData.heading;
		data.Pitch = 0;
		data.Roll = 0;
		//unused: speed

		fg.ProcessTick(data, FGGetTime());

		double delta = NOW_MILLI - begin;
		std::this_thread::sleep_for(std::chrono::milliseconds(FGSendRateMs - (long long)delta));
	}
#undef NOW_MILLI
}

void App::SCUpdateData(SCClient::AirplaneData& data)
{
	airplaneData = data;
}

double App::FGGetTime()
{
	auto now = std::chrono::system_clock::now().time_since_epoch();
	auto today = std::chrono::floor<std::chrono::days>(now);
	now = now - today;
	return now.count() / 10000000.0;
}
