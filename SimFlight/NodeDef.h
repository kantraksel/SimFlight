#pragma once
#include <cstring>

namespace ND
{
	enum ValueType
	{
		VT_Unknown,
		VT_Integer,
		VT_Float,
		VT_String,
	};

	union GenericValue
	{
		int intVal;
		float floatVal;
		unsigned int uintVal;

		GenericValue() : intVal(0) {}
		GenericValue(long value) : intVal(value) {}
		GenericValue(int value) : intVal(value) {}
		GenericValue(float value) : floatVal(value) {}
		GenericValue(double value) : floatVal(value) {}

		operator int()
		{
			return intVal;
		}

		operator float()
		{
			return floatVal;
		}

		operator unsigned int()
		{
			return uintVal;
		}
	};

	struct NodeGeneric
	{
		unsigned int id;
		GenericValue value;
		unsigned char* pValue2;

		unsigned int pValue2Capacity;
		ValueType valueType;

		NodeGeneric()
		{
			memset(this, 0, sizeof(*this));
		}
	};
}
