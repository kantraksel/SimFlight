#include "FlightSimulatorState.h"

FlightSimulatorState::FlightSimulatorState()
{
	Reset();
}

void FlightSimulatorState::UpdateSimulationState(bool state, StateChange callback)
{
	isSimulating = state;
	UpdateState(callback);
}

void FlightSimulatorState::UpdatePauseState(bool state, StateChange callback)
{
	isPaused = state;
	UpdateState(callback);
}

bool FlightSimulatorState::IsRunning()
{
	return isRunning;
}

void FlightSimulatorState::UpdateState(StateChange callback)
{
	bool wasRunning = isRunning;
	isRunning = isSimulating && !isPaused;

	if (wasRunning ^ isRunning)
		callback(isRunning);
}

void FlightSimulatorState::Reset()
{
	isSimulating = false;
	isPaused = false;
	isRunning = false;
}
