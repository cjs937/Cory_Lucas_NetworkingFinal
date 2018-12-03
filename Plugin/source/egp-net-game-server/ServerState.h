#pragma once
#include <map>
#include <vector>
#include <queue>
#include <chrono>
#include "egp-raknet-console/PlayerData.h"
#include "egp-net-framework/Entity.h"

//class DemoState;
class DemoPeerManager;
class InputManager;

namespace RakNet
{
	struct Packet;
}

class ServerState
{
private:
	//DemoState* localState;
	std::vector<PlayerData> dataThisFrame;

	bool runLoop;
	int latencyThreshold;

	//Timing
	std::chrono::time_point<std::chrono::system_clock> lastTime;
	std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> lastTimeMS;
	std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> lastNetworkUpdateMS;

	void checkWorldCollisions();

public:
	ServerState();
	~ServerState();

	bool init();

	void updateState();

	//inline bool shouldSendState() { return sendGameState; };
	//inline bool shouldSendState(bool _flag) { return sendGameState = _flag; };
	void exitLoop();
	bool shouldLoop();
	ServerState* getInstance();
	void addPlayerData(PlayerData _data);
};

