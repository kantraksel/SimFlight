#pragma once

namespace Maths
{
	template <typename T> struct Vec3
	{
		T x;
		T y;
		T z;

		Vec3<T> operator*(T factor)
		{
			return Vec3<T>{ x * factor, y * factor, z * factor };
		}
	};

	typedef Vec3<double> Vec3d;
	typedef Vec3<float> Vec3f;

	struct Geod
	{
		static constexpr double FeetToMeter = 0.3048;

		double longitude;
		double latitude;
		double elevation;

		Geod(double longRad, double latRad, double elevFt) :
			longitude(longRad), latitude(latRad), elevation(elevFt * FeetToMeter)
		{}

		inline double getLongitudeRad() const
		{
			return longitude;
		}

		inline double getLatitudeRad() const
		{
			return latitude;
		}

		inline double getElevationM() const
		{
			return elevation;
		}
	};

	struct Quaternion
	{
		float x;
		float y;
		float z;
		float w;

		inline void getAngleAxis(Vec3f& axis) const
		{
			float angle;
			getAngleAxis(angle, axis);
			axis = axis * angle;
		}

		void getAngleAxis(float& angle, Vec3f& axis) const;
	};

	inline Quaternion operator*(const Quaternion& v1, const Quaternion& v2)
	{
		Quaternion v;
		v.x = v1.w * v2.x + v1.x * v2.w + v1.y * v2.z - v1.z * v2.y;
		v.y = v1.w * v2.y - v1.x * v2.z + v1.y * v2.w + v1.z * v2.x;
		v.z = v1.w * v2.z + v1.x * v2.y - v1.y * v2.x + v1.z * v2.w;
		v.w = v1.w * v2.w - v1.x * v2.x - v1.y * v2.y - v1.z * v2.z;
		return v;
	}

	void GeodToCart(const Geod& geod, Vec3d& cart);
	Quaternion EarthRadToLocalFrame(float lon, float lat);
	Quaternion EulerRadToQuat(float z, float y, float x);
}
