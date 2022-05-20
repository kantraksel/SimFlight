#pragma once
#include <functional>

class FlightSimulatorState
{
	public:
		typedef std::function<void(bool enabled)> StateChange;

	private:
		bool isSimulating;
		bool isPaused;
		bool isRunning;

		void UpdateState(StateChange callback);

	public:
		FlightSimulatorState();
		void UpdateSimulationState(bool state, StateChange callback);
		void UpdatePauseState(bool state, StateChange callback);
		void Reset();

		bool IsRunning();
};
