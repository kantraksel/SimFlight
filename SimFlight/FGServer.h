#pragma once
class FGServer
{
	public:
		struct AirplaneData
		{
			double Longitude;
			double Latitude;
			double Altitude;

			float Heading;
			float Pitch;
			float Roll;
		};

	private:
		void* pSocket;
		unsigned char packetBuff[1500];

	public:
		FGServer();
		~FGServer();

		void SelectServer(const char* host, unsigned short port);
		void SetPeer(const char* callsign, const char* aircraft);

		void ProcessTick(AirplaneData& geoData, double time);
};
