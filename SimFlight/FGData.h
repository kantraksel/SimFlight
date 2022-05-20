#pragma once

namespace FGData
{
	struct ValueDef
	{
		unsigned int id;
		ND::ValueType type;
		const char* szName;
		ND::GenericValue defaultValue;
	};

	constexpr int PropertyCount = 41;
	extern ValueDef Properties[PropertyCount];
}

