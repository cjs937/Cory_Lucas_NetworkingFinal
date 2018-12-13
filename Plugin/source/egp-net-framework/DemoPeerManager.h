/*
	egpNetServerPeerManager.h
	By Dan Buckstein
	(c) 2017-2018

	Specialized peer manager for server.

	Additional contributions by (and date): 

*/

#ifndef __EGPNET_SERVERPEERMANAGER_H_
#define __EGPNET_SERVERPEERMANAGER_H_

#include "egp-net/fw/egpNetPeerManager.h"
#include <vector>
#include <queue>
#include <mutex>
#include "ClientID.h"

struct PlayerData;

struct CombatPlayerData
{
	PlayerData* playerData;
	ClientID opponentID;
};

typedef std::pair<RakNet::SystemAddress, ClientID> idAdressPair;

// server manager
class DemoPeerManager : public egpNetPeerManager
{
	// packet individual packet; function should not change manager
	//	packet: pointer to packet
	//	packetIndex: index in sequence of processed packets
	// return 0 to stop processing packets
	virtual int ProcessPacket(const RakNet::Packet *const packet, const unsigned int packetIndex);

public: 

	enum DemoPackets
	{
		e_id_packetBegin = ID_USER_PACKET_ENUM,

		ID_GAME_MESSAGE_1,
		UPDATE_NETWORK_PLAYER, // Client sends server a single entity
		UPDATE_GAME_STATE, // Server sends client update of all current entities
		PLAYER_COLLISION,
		PLAYER_WIN_ROUND,
		UPDATE_COMBAT_PLAYER,
		SERVER_SHUTDOWN,
		// end; indicates where specialized managers can start
		e_id_packetEnd
	};

	std::mutex dataLock;
	std::vector<PlayerData*> pendingPlayerUpdates;
	std::vector<CombatPlayerData*> pendingAttackers;

	//system address & clientID
	std::vector<idAdressPair> idAdressPairs;
	// ctor
	DemoPeerManager() = default;

	// dtor
	virtual ~DemoPeerManager();

	static DemoPeerManager* getInstance();
	std::string serverAddress;
	inline int getConnectedClients() { return mp_peer->NumberOfConnections(); };
	

	const unsigned int serverPort = 4040;
	// Time between ticks in MS - only used for sending
	const float networkTickRateMS = 100.0f;

	RakNet::Time latencyThreshold = 1000;

	RakNet::RakPeerInterface* getPeer()
	{
		return mp_peer;
	}

	// Our functions called from the plugin
	void sendEntity(RakNet::BitStream* bs, int peer = -1) const;

	bool checkLatency(RakNet::Time latency);

	PlayerData* createPlayerFromPacket(RakNet::BitStream* _entityData);

	static RakNet::Time calcLatency(RakNet::Time _timeRecieved, RakNet::Time _timeSent);

	///<summary>
	///Assumes latency data is next in stream. MessageID and any other previous data must be stripped
	///</summary>
	static RakNet::Time calcLatency(RakNet::BitStream& _stream); 

	void addPlayerData(PlayerData* _data);
	void addCombatData(CombatPlayerData* _data);
	bool setClientIDPair(RakNet::SystemAddress, ClientID _id);
	bool hasClientIDPair(RakNet::SystemAddress _address);
	idAdressPair getClientIDPair(RakNet::SystemAddress);
	void onPlayerDisconnect(RakNet::SystemAddress _systemAddress);
	bool removeFromIDPairList(RakNet::SystemAddress _address);
};


#endif	// !__EGPNET_SERVERPEERMANAGER_H_