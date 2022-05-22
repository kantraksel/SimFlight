#include <chrono>
#include <thread>
#include "Logger.h"
#include "SocketApi.h"
#include "App.h"
#include "version.h"

void App::Main()
{
	InitEnv();
	PrepareClient();
	PrepareAirplane();

	isAppRunning = true;
	std::thread clientThread(&App::SimConnectThread, this);
	std::thread airplaneThread(&App::AirplaneThread, this);
	Logger::Log("Ready!");

	Logger::Log("Press any key to exit");
	std::ignore = getchar();

	Logger::Log("Shutting down");
	isAppRunning = false;
	client.Disconnnect();
	airplaneThread.join();
	clientThread.join();

	SocketApi::Shutdown();
}

void App::InitEnv()
{
	Logger::Init();
	Logger::Log("SimFlight " APP_VERSION);

	Logger::Log("Initializing...");
	SocketApi::Init();

	if (!config.Load())
	{
		Logger::Log("Failed to load config!");
		std::exit(1);
	}
	Logger::Log("Loaded config");
}

void App::PrepareClient()
{
	client.SetEvent(&airplane);
	if (!client.Connect())
	{
		Logger::Log("Failed to connect to Flight Simulator!");
		std::exit(1);
	}
	Logger::Log("Connected to Flight Simulator");
}

void App::PrepareAirplane()
{
	FGServer& server = airplane.GetServer();
	auto ip = config.GetString(Config::Section::FGConn, "Ip");
	auto port = config.GetInt(Config::Section::FGConn, "Port");

	auto callsign = config.GetString(Config::Section::General, "Callsign");
	auto aircraft = config.GetString(Config::Section::Aircraft, "Path");
	auto livery = config.GetString(Config::Section::Aircraft, "Livery");
	auto fallbackId = config.GetInt(Config::Section::Aircraft, "FallbackId");

	Logger::Log("");
	Logger::Log(std::format("Callsign {}", callsign));
	Logger::Log(std::format("Airplane {}", aircraft));
	Logger::Log(std::format("Livery {}", livery));
	Logger::Log("");

	server.SelectServer(ip.c_str(), port);
	server.SetPeer(callsign.c_str(), aircraft.c_str(), livery.c_str(), fallbackId);
	Logger::Log("Loaded airplane data");
}

#pragma region Threads

void App::SimConnectThread()
{
	client.RunLoop();
	isAppRunning = false;
	Logger::Log("Flight Simulator: Thread ended");
}

void App::AirplaneThread()
{
#define NOW_MILLI (std::chrono::steady_clock::now().time_since_epoch().count() * NanoToMilli)
	constexpr double NanoToMilli = 0.000001;
	constexpr long long SendRateMs = 100;

	while (isAppRunning)
	{
		double begin = NOW_MILLI;

		airplane.OnTick();

		double delta = NOW_MILLI - begin;
		std::this_thread::sleep_for(std::chrono::milliseconds(SendRateMs - (long long)delta));
	}

	Logger::Log("Airplane: Thread ended");
}

#pragma endregion
