#include "NodeDef.h"
#include "FGData.h"

using namespace FGData;
using namespace ND;

ValueDef FGData::Properties[PropertyCount] = {
		{ 100, ValueType::VT_Float, "surface-positions/left-aileron-pos-norm", 0.0f },
		{ 101, ValueType::VT_Float, "surface-positions/right-aileron-pos-norm", 0.0f },
		{ 102, ValueType::VT_Float, "surface-positions/elevator-pos-norm", 0.0f },
		{ 103, ValueType::VT_Float, "surface-positions/rudder-pos-norm", 0.0f },
		{ 104, ValueType::VT_Float, "surface-positions/flap-pos-norm", 0.0f },
		{ 105, ValueType::VT_Float, "surface-positions/speedbrake-pos-norm", 0.0f },

		{ 200, ValueType::VT_Float, "gear/gear[0]/compression-norm", 0.0f },
		{ 201, ValueType::VT_Float, "gear/gear[0]/position-norm", 0.0f },
		{ 210, ValueType::VT_Float, "gear/gear[1]/compression-norm", 0.0f },
		{ 211, ValueType::VT_Float, "gear/gear[1]/position-norm", 0.0f },
		{ 220, ValueType::VT_Float, "gear/gear[2]/compression-norm", 0.0f },
		{ 221, ValueType::VT_Float, "gear/gear[2]/position-norm", 0.0f },
		{ 230, ValueType::VT_Float, "gear/gear[3]/compression-norm", 0.0f },
		{ 231, ValueType::VT_Float, "gear/gear[3]/position-norm", 0.0f },
		{ 240, ValueType::VT_Float, "gear/gear[4]/compression-norm", 0.0f },
		{ 241, ValueType::VT_Float, "gear/gear[4]/position-norm", 0.0f },

		{ 300, ValueType::VT_Float, "engines/engine[0]/n1", 0.0f },
		{ 301, ValueType::VT_Float, "engines/engine[0]/n2", 0.0f },
		{ 302, ValueType::VT_Float, "engines/engine[0]/rpm", 0.0f },
		{ 310, ValueType::VT_Float, "engines/engine[1]/n1", 0.0f },
		{ 311, ValueType::VT_Float, "engines/engine[1]/n2", 0.0f },
		{ 312, ValueType::VT_Float, "engines/engine[1]/rpm", 0.0f },
		{ 320, ValueType::VT_Float, "engines/engine[2]/n1", 0.0f },
		{ 321, ValueType::VT_Float, "engines/engine[2]/n2", 0.0f },
		{ 322, ValueType::VT_Float, "engines/engine[2]/rpm", 0.0f },
		{ 330, ValueType::VT_Float, "engines/engine[3]/n1", 0.0f },
		{ 331, ValueType::VT_Float, "engines/engine[3]/n2", 0.0f },
		{ 332, ValueType::VT_Float, "engines/engine[3]/rpm", 0.0f },

		{ 1001, ValueType::VT_Float, "controls/flight/slats", 0.0f },
		{ 1002, ValueType::VT_Float, "controls/flight/speedbrake", 0.0f },
		{ 1003, ValueType::VT_Float, "controls/flight/spoilers", 0.0f },
		{ 1004, ValueType::VT_Float, "controls/gear/gear-down", 0.0f },
		{ 1005, ValueType::VT_Float, "controls/lighting/nav-lights", 0.0f },

		{ 1101, ValueType::VT_String, "sim/model/livery/file", 0l },
		{ 1500, ValueType::VT_Integer, "instrumentation/transponder/transmitted-id", 0l },
		{ 10002, ValueType::VT_String, "sim/multiplay/chat", 0l },
		{ 11990, ValueType::VT_Integer, "sim/multiplay/mp-clock-mode", 0l },
		{ 13000, ValueType::VT_Integer, "sim/model/fallback-model-index", 0l },
};
