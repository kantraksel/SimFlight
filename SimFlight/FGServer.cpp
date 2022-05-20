#include "FGServer.h"
#include <chrono>
#include <numbers>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "WNET/include/wnet.h"
#include "FlightGear/tiny_xdr.hxx"
#include "Maths.h"
#include "Logger.h"

#define HSOCKET(pSocket) ((WNET::IClientSocket*)pSocket)
constexpr uint32_t Magic = 0x46474653;
constexpr uint32_t Version = 0x00010001;
constexpr uint32_t MsgId_Position = 7;
constexpr double DegreesToRadians = std::numbers::pi / 180.0;

struct Header
{
	xdr_data_t Magic;
	xdr_data_t Version;
	xdr_data_t Id;
	xdr_data_t Lenght;
	xdr_data_t RequestedRange;
	xdr_data_t Reserved0;
	char Callsign[8];
};

struct PositionMsg
{
	Header header;

	char Model[96];

	xdr_data2_t Time;
	xdr_data2_t Lag;

	xdr_data2_t Position[3];
	xdr_data_t Orientation[3];

	xdr_data_t LinearVelocity[3];
	xdr_data_t AngularVelocity[3];
	xdr_data_t LinearAcceleration[3];
	xdr_data_t AngularAcceleration[3];

	xdr_data_t Reserved0;
};

FGServer::FGServer()
{
	pSocket = WNET::IClientSocket::Create();

	PositionMsg& msg = *(PositionMsg*)packetBuff;
	Header& header = msg.header;
	ZeroMemory(&msg, sizeof(msg));
	header.Magic = XDR_encode_uint32(Magic);
	header.Version = XDR_encode_uint32(Version);
	header.Id = XDR_encode_uint32(MsgId_Position);
	header.Lenght = XDR_encode_uint32(sizeof(PositionMsg));
	header.RequestedRange = XDR_encode_uint32(100);
	header.Reserved0 = 0;

	body.SetNode(11990, 1);
}

FGServer::~FGServer()
{
	delete pSocket;
}

void FGServer::SelectServer(const char* host, unsigned short port)
{
	HSOCKET(pSocket)->Connect(host, port);
}

void FGServer::SetPeer(const char* callsign, const char* aircraft, const char* livery, long fallbackId)
{
	static PositionMsg& msg = *(PositionMsg*)packetBuff;
	static Header& header = msg.header;

	memcpy_s(&header.Callsign, sizeof(header.Callsign), callsign, strlen(callsign));
	memcpy_s(&msg.Model, sizeof(msg.Model), aircraft, strlen(aircraft));
	body.SetNode(1101, livery, strlen(livery));
	body.SetNode(13000, fallbackId);

	body.SetNode(13000, 252);
}

struct CartesianPos
{
	Maths::Vec3d Position;
	Maths::Vec3f Orientation;
};

static void GeoToCartPos(AirplaneData& geoData, CartesianPos& cartData)
{
	double longitude = geoData.longitude * DegreesToRadians;
	double latitude = geoData.latitude * DegreesToRadians;
	Maths::Geod geod(longitude, latitude, geoData.altitude);
	Maths::GeodToCart(geod, cartData.Position);

	geoData.heading *= DegreesToRadians;
	geoData.pitch *= DegreesToRadians;
	geoData.roll *= DegreesToRadians;
	auto localFrame = Maths::EarthRadToLocalFrame(longitude, latitude);
	auto orientation = Maths::EulerRadToQuat(geoData.heading, geoData.pitch, geoData.roll);
	orientation = localFrame * orientation;
	orientation.getAngleAxis(cartData.Orientation);
}

static double GetTime()
{
	auto now = std::chrono::system_clock::now().time_since_epoch();
	auto today = std::chrono::floor<std::chrono::days>(now);
	now = now - today;
	return now.count() / 10000000.0;
}

void FGServer::ProcessTick(AirplaneData geoData)
{
	static PositionMsg& msg = *(PositionMsg*)packetBuff;
	static Header& header = msg.header;

	geoData.pitch = -geoData.pitch;
	geoData.roll = -geoData.roll;

	CartesianPos cartData;
	GeoToCartPos(geoData, cartData);
	
	msg.Time = XDR_encode_double(GetTime());
	msg.Lag = XDR_encode_double(0.10000000000000001); //seems to be const - check along much more data
	msg.Position[0] = XDR_encode_double(cartData.Position.x);
	msg.Position[1] = XDR_encode_double(cartData.Position.y);
	msg.Position[2] = XDR_encode_double(cartData.Position.z);
	msg.Orientation[0] = XDR_encode_float(cartData.Orientation.x);
	msg.Orientation[1] = XDR_encode_float(cartData.Orientation.y);
	msg.Orientation[2] = XDR_encode_float(cartData.Orientation.z);

	body.SetNode(100, geoData.aileronLeft);
	body.SetNode(101, -geoData.aileronRight);
	body.SetNode(102, -geoData.elevator);
	body.SetNode(103, geoData.rudder);
	body.SetNode(104, geoData.flaps);
	body.SetNode(105, geoData.spoilersLeft);

	body.SetNode(201, geoData.gearExtendedPct);
	body.SetNode(211, geoData.gearExtendedPct);
	body.SetNode(221, geoData.gearExtendedPct);
	body.SetNode(231, geoData.gearExtendedPct);
	body.SetNode(241, geoData.gearExtendedPct);

	geoData.engine1n1 /= AirplaneData::EngineRPMMax;
	geoData.engine1n2 /= AirplaneData::EngineRPMMax;
	geoData.engine2n1 /= AirplaneData::EngineRPMMax;
	geoData.engine2n2 /= AirplaneData::EngineRPMMax;
	geoData.engine3n1 /= AirplaneData::EngineRPMMax;
	geoData.engine3n2 /= AirplaneData::EngineRPMMax;
	geoData.engine4n1 /= AirplaneData::EngineRPMMax;
	geoData.engine4n2 /= AirplaneData::EngineRPMMax;

	body.SetNode(300, geoData.engine1n1);
	body.SetNode(301, geoData.engine1n2);
	body.SetNode(302, geoData.engine1n1);
	body.SetNode(310, geoData.engine2n1);
	body.SetNode(311, geoData.engine2n2);
	body.SetNode(312, geoData.engine2n1);
	body.SetNode(320, geoData.engine3n1);
	body.SetNode(321, geoData.engine3n2);
	body.SetNode(322, geoData.engine3n1);
	body.SetNode(330, geoData.engine4n1);
	body.SetNode(331, geoData.engine4n2);
	body.SetNode(332, geoData.engine4n1);

	body.SetNode(1500, geoData.squawkCode);

	int bodySize = body.WriteBody(packetBuff + sizeof(PositionMsg), sizeof(packetBuff) - sizeof(PositionMsg));
	if (bodySize == -1)
	{
		Logger::Log("FGServer::ProcessTick() - body overflow. Body has been cleared to avoid error spam");
		body.Clear();
		bodySize = 0;
	}

	header.Lenght = XDR_encode_uint32(sizeof(PositionMsg) + bodySize);
	HSOCKET(pSocket)->Send(packetBuff, sizeof(PositionMsg) + bodySize);
}
