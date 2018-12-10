#pragma once
#include <map>
#include <vector>
#include <queue>
#include <chrono>
//#include "egp-net-framework/PlayerData.h"
//#include "egp-net-framework/Entity.h"

//class DemoState;
class DemoPeerManager;
class InputManager;
struct PlayerData;
struct ClientID;

namespace RakNet
{
	struct Packet;
	struct BitStream;
}

class ServerState
{
private:
	//DemoState* localState;
	std::vector<PlayerData*> dataThisFrame;
	std::vector<PlayerData*> pendingAttackers;

	bool runLoop;
	int latencyThreshold;

	//Timing
	std::chrono::time_point<std::chrono::system_clock> lastTime;
	std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> lastTimeMS;
	std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> lastNetworkUpdateMS;

	void checkWorldCollisions();
	void sendCollisionPacket(ClientID _p1, ClientID _p2);
	bool canPerformAttacks(PlayerData* _player, ClientID _opponentID);
	void handlePlayerBattle(PlayerData* _player1, PlayerData* _player2);
	void sendRoundWinnerPacket(ClientID _winner, ClientID _loser, bool isDraw = false);

public:
	ServerState();
	~ServerState();

	bool init();

	void updateState();

	//inline bool shouldSendState() { return sendGameState; };
	//inline bool shouldSendState(bool _flag) { return sendGameState = _flag; };
	void exitLoop();
	bool shouldLoop();
	void addPlayerData(PlayerData* _data);

	///<summary>
	/// Assumes Message ID is already stripped from bitstream
	///</summary>
	void addPlayerData(RakNet::BitStream* _entityData); 

	///<summary>
	/// Assumes Message ID is already stripped from bitstream
	///</summary>
	void handleAttacker(RakNet::BitStream* _entityData, ClientID _opponentID); // TODO: SAME HERE

	///<summary>
	/// Assumes Message ID is already stripped from bitstream
	///</summary>
	PlayerData* createPlayerFromPacket(RakNet::BitStream* _entityData);

	static ServerState* getInstance();
};

