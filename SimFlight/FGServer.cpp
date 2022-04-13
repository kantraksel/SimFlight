#include "FGServer.h"
#include <numbers>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "WNET/include/wnet.h"
#include "FlightGear/tiny_xdr.hxx"
#include "Maths.h"

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

//tmp:
struct Property
{
	xdr_data_t id;
	xdr_data_t strlen;
	xdr_data_t str[3]; //3=strlen
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
}

FGServer::~FGServer()
{
	delete pSocket;
}

void FGServer::SelectServer(const char* host, unsigned short port)
{
	HSOCKET(pSocket)->Connect(host, port);
}

void FGServer::SetPeer(const char* callsign, const char* aircraft)
{
	static PositionMsg& msg = *(PositionMsg*)packetBuff;
	static Header& header = msg.header;

	memcpy_s(&header.Callsign, sizeof(header.Callsign), callsign, strlen(callsign));
	memcpy_s(&msg.Model, sizeof(msg.Model), aircraft, strlen(aircraft));
}

struct CartesianPos
{
	Maths::Vec3d Position;
	Maths::Vec3f Orientation;
};

void GeoToCartPos(FGServer::AirplaneData& geoData, CartesianPos& cartData)
{
	double longitude = geoData.Longitude * DegreesToRadians;
	double latitude = geoData.Latitude * DegreesToRadians;
	Maths::Geod geod(longitude, latitude, geoData.Altitude);
	Maths::GeodToCart(geod, cartData.Position);

	geoData.Heading *= DegreesToRadians;
	geoData.Pitch *= DegreesToRadians;
	geoData.Roll *= DegreesToRadians;
	auto localFrame = Maths::EarthRadToLocalFrame(longitude, latitude);
	auto orientation = Maths::EulerRadToQuat(geoData.Heading, geoData.Pitch, geoData.Roll);
	orientation = localFrame * orientation;
	orientation.getAngleAxis(cartData.Orientation);
}

void FGServer::ProcessTick(AirplaneData& geoData, double time)
{
	static PositionMsg& msg = *(PositionMsg*)packetBuff;
	static Header& header = msg.header;

	CartesianPos cartData;
	GeoToCartPos(geoData, cartData);
	
	msg.Time = XDR_encode_double(time);
	msg.Lag = XDR_encode_double(0.10000000000000001); //seems to be const - check along much more data
	msg.Position[0] = XDR_encode_double(cartData.Position.x);
	msg.Position[1] = XDR_encode_double(cartData.Position.y);
	msg.Position[2] = XDR_encode_double(cartData.Position.z);
	msg.Orientation[0] = XDR_encode_float(cartData.Orientation.x);
	msg.Orientation[1] = XDR_encode_float(cartData.Orientation.y);
	msg.Orientation[2] = XDR_encode_float(cartData.Orientation.z);

	// accels are always zero
	/*
	if paused - zeros
	else
	motionInfo.linearVel = SG_FEET_TO_METER*SGVec3f(ifce.get_uBody(),
			ifce.get_vBody(),
			ifce.get_wBody());
		motionInfo.angularVel = SGVec3f(ifce.get_P_body(),
			ifce.get_Q_body(),
			ifce.get_R_body());
	*/

	//static Property& prop = *(Property*)(packetBuff + sizeof(PositionMsg));
	//prop.id = XDR_encode_uint32(0x044D); //livery
	//prop.id = XDR_encode_uint32(0x2712); //chat
	//prop.strlen = XDR_encode_uint32(3);
	//prop.str[0] = XDR_encode_int8('K');
	//prop.str[1] = XDR_encode_int8('L');
	//prop.str[2] = XDR_encode_int8('M');
	
	HSOCKET(pSocket)->Send(packetBuff, sizeof(PositionMsg));
}
