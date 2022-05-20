#pragma once

struct AirplaneData
{
	double longitude;
	double latitude;
	double altitude;

	double heading;
	double pitch;
	double roll;

	double aileronLeft;
	double aileronRight;
	double elevator;
	double rudder;
	double flaps;
	double spoilersLeft;
	//double spoilersRight;

	double engine1n1;
	double engine1n2;
	double engine2n1;
	double engine2n2;
	double engine3n1;
	double engine3n2;
	double engine4n1;
	double engine4n2;

	double gearExtendedPct;

	double squawkCode;

	constexpr static double EngineRPMMax = 10;
};
