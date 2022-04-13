#include "Maths.h"
#include <cmath>
#include <limits>

using namespace Maths;

#define _SQUASH 0.9966471893352525192801545
constexpr double a = 6378137.0;
static double e2 = fabs(1 - _SQUASH * _SQUASH);
constexpr float PI = 3.1415926535897932384626433832795029L;

void Maths::GeodToCart(const Geod& geod, Vec3d& cart)
{
	// according to
	// H. Vermeille,
	// Direct transformation from geocentric to geodetic ccordinates,
	// Journal of Geodesy (2002) 76:451-454
	double lambda = geod.getLongitudeRad();
	double phi = geod.getLatitudeRad();
	double h = geod.getElevationM();
	double sphi = sin(phi);
	double n = a / sqrt(1 - e2 * sphi * sphi);
	double cphi = cos(phi);
	double slambda = sin(lambda);
	double clambda = cos(lambda);
	cart.x = (h + n) * cphi * clambda;
	cart.y = (h + n) * cphi * slambda;
	cart.z = (h + n - e2 * n) * sphi;
}

//NOTE: will be needed - fg to fsx sync 
/*
void
SGGeodesy::SGCartToGeod(const SGVec3<double>& cart, SGGeod& geod)
{
  // according to
  // H. Vermeille,
  // Direct transformation from geocentric to geodetic ccordinates,
  // Journal of Geodesy (2002) 76:451-454
  double X = cart(0);
  double Y = cart(1);
  double Z = cart(2);
  double XXpYY = X*X+Y*Y;
  if( XXpYY + Z*Z < 25 ) {
	// This function fails near the geocenter region, so catch that special case here.
	// Define the innermost sphere of small radius as earth center and return the
	// coordinates 0/0/-EQURAD. It may be any other place on geoide's surface,
	// the Northpole, Hawaii or Wentorf. This one was easy to code ;-)
	geod.setLongitudeRad( 0.0 );
	geod.setLatitudeRad( 0.0 );
	geod.setElevationM( -EQURAD );
	return;
  }

  double sqrtXXpYY = sqrt(XXpYY);
  double p = XXpYY*ra2;
  double q = Z*Z*(1-e2)*ra2;
  double r = 1/6.0*(p+q-e4);
  double s = e4*p*q/(4*r*r*r);
/*
  s*(2+s) is negative for s = [-2..0]
  slightly negative values for s due to floating point rounding errors
  cause nan for sqrt(s*(2+s))
  We can probably clamp the resulting parable to positive numbers
*
if (s >= -2.0 && s <= 0.0)
s = 0.0;
double t = pow(1 + s + sqrt(s * (2 + s)), 1 / 3.0);
double u = r * (1 + t + 1 / t);
double v = sqrt(u * u + e4 * q);
double w = e2 * (u + v - q) / (2 * v);
double k = sqrt(u + v + w * w) - w;
double D = k * sqrtXXpYY / (k + e2);
geod.setLongitudeRad(2 * atan2(Y, X + sqrtXXpYY));
double sqrtDDpZZ = sqrt(D * D + Z * Z);
geod.setLatitudeRad(2 * atan2(Z, D + sqrtDDpZZ));
geod.setElevationM((k + e2 - 1) * sqrtDDpZZ / k);
}
*/

Quaternion Maths::EarthRadToLocalFrame(float lon, float lat)
{
	Quaternion q;
	float zd2 = float(0.5) * lon;
	float yd2 = float(-0.25) * PI - float(0.5) * lat;
	float Szd2 = sin(zd2);
	float Syd2 = sin(yd2);
	float Czd2 = cos(zd2);
	float Cyd2 = cos(yd2);
	q.w = Czd2 * Cyd2;
	q.x = -Szd2 * Syd2;
	q.y = Czd2 * Syd2;
	q.z = Szd2 * Cyd2;
	return q;
}

Quaternion Maths::EulerRadToQuat(float z, float y, float x)
{
	Quaternion q;
	float zd2 = 0.5f * z; float yd2 = 0.5f * y; float xd2 = 0.5f * x;
	float Szd2 = sin(zd2); float Syd2 = sin(yd2); float Sxd2 = sin(xd2);
	float Czd2 = cos(zd2); float Cyd2 = cos(yd2); float Cxd2 = cos(xd2);
	float Cxd2Czd2 = Cxd2 * Czd2; float Cxd2Szd2 = Cxd2 * Szd2;
	float Sxd2Szd2 = Sxd2 * Szd2; float Sxd2Czd2 = Sxd2 * Czd2;
	q.w = Cxd2Czd2 * Cyd2 + Sxd2Szd2 * Syd2;
	q.x = Sxd2Czd2 * Cyd2 - Cxd2Szd2 * Syd2;
	q.y = Cxd2Czd2 * Syd2 + Sxd2Szd2 * Cyd2;
	q.z = Cxd2Szd2 * Cyd2 - Sxd2Czd2 * Syd2;
	return q;
}

inline float dot(const Quaternion& v1, const Quaternion& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}
inline float norm(const Quaternion& v)
{
	return sqrt(dot(v, v));
}

inline Vec3f imag(const Quaternion& v)
{
	return Vec3f{v.x, v.y, v.z};
}

float max(const float& a, const float& b)
{
	return a > b ? a : b;
}

float min(const float& a, const float& b)
{
	return a < b ? a : b;
}

void Maths::Quaternion::getAngleAxis(float& angle, Vec3f& axis) const
{
	float nrm = norm(*this);
	if (nrm <= std::numeric_limits<float>::min()) {
		angle = 0;
		axis = Vec3f{ 0, 0, 0 };
	}
	else {
		float rNrm = 1 / nrm;
		angle = acos(max(-1, min(1, rNrm * w)));
		float sAng = sin(angle);
		if (fabs(sAng) <= std::numeric_limits<float>::min())
			axis = Vec3f{ 1, 0, 0 };
		else
			axis = imag(*this) * (rNrm / sAng);
		angle *= 2;
	}
}
