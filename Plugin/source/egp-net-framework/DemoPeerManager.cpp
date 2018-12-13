/*
	egpNetServerPeerManager.h
	By Dan Buckstein
	(c) 2017-2018

	Server manager source.

	Additional contributions by (and date): 

*/

#include "DemoPeerManager.h"
#include "Vector3.h"
#include <iostream>
#include <mutex>
#include "../egp-net-game-server/ServerState.h"
#include "PlayerData.h"

int DemoPeerManager::ProcessPacket(const RakNet::Packet *const packet, const unsigned int packetIndex)
{
	// ****TO-DO: implement server-specific packet processor

	switch (packet->data[0])
	{
	case ID_REMOTE_DISCONNECTION_NOTIFICATION:
		printf("Another client has disconnected.\n");
		break;
	case ID_REMOTE_CONNECTION_LOST:
		printf("Another client has lost the connection.\n");
		break;
	case ID_REMOTE_NEW_INCOMING_CONNECTION:
		printf("Another client has connected.\n");
		break;
	case ID_NEW_INCOMING_CONNECTION:
		printf("A connection is incoming.\n");
		break;
	case ID_NO_FREE_INCOMING_CONNECTIONS:
		printf("The server is full.\n");
		break;
	case ID_DISCONNECTION_NOTIFICATION:
		
		break;
	case ID_CONNECTION_LOST:
		
		break;
	case UPDATE_NETWORK_PLAYER:
	printf("Updating network player:");
	{
		RakNet::BitStream stream(packet->data, packet->length, false);

		sendEntity(&stream, mp_peer->GetIndexFromSystemAddress(packet->systemAddress));
		
		stream.IgnoreBytes(sizeof(RakNet::MessageID));
		stream.IgnoreBytes(sizeof(RakNet::Time));

		//ServerState::getInstance()->addPlayerData(&stream);
		std::lock_guard<std::mutex> lock(DemoPeerManager::dataLock);
		pendingPlayerUpdates.push_back(createPlayerFromPacket(&stream));

		std::cout << "Done" << std::endl;
	}
		break;
	case UPDATE_COMBAT_PLAYER:
	{
		RakNet::BitStream stream(packet->data, packet->length, false);

		stream.IgnoreBytes(sizeof(RakNet::MessageID));

		PlayerData* player = createPlayerFromPacket(&stream);

		ClientID opponentId(&stream);

		CombatPlayerData* combatPlayer = new CombatPlayerData();

		combatPlayer->playerData = player;
		combatPlayer->opponentID = opponentId;


		std::lock_guard<std::mutex> lock(DemoPeerManager::dataLock);
		pendingAttackers.push_back(combatPlayer);

		break;
	}
	default:
		std::cout << "ID" << packet->data[0] << std::endl;
		break;
	}

	return 0;
}

void DemoPeerManager::sendEntity(RakNet::BitStream* bs, int peer) const
{
	SendPacket(bs, peer, true, true);
}


DemoPeerManager::~DemoPeerManager()
{
	for (int i = 0; i < pendingPlayerUpdates.size(); ++i)
	{
		delete pendingPlayerUpdates[i];
	}

	for (int i = 0; i < pendingAttackers.size(); ++i)
	{
		delete pendingAttackers[i];
	}
}

DemoPeerManager* DemoPeerManager::getInstance()
{
	static DemoPeerManager instance;

	return &instance;
}

bool DemoPeerManager::checkLatency(RakNet::Time latency)
{
	if (latency >= latencyThreshold)
		return false;

	return true;
}

RakNet::Time DemoPeerManager::calcLatency(RakNet::Time _timeRecieved, RakNet::Time _timeSent)
{
	return _timeRecieved - _timeSent;
}

RakNet::Time DemoPeerManager::calcLatency(RakNet::BitStream& _stream)
{
	RakNet::Time theirT, ourT;
	ourT = RakNet::GetTime();
	_stream.Read(theirT);
	
	return calcLatency(ourT, theirT);
}

PlayerData* DemoPeerManager::createPlayerFromPacket(RakNet::BitStream* _entityData)
{
	//int guidLength;
	Vector3 position;
	Vector3 destination;
	float collisionRadius;
	bool inCombat;
	int currentAttack;
	ClientID id;

	//_entityData.IgnoreBytes(sizeof((char)DemoPeerManager::UPDATE_NETWORK_PLAYER));

	id = ClientID(_entityData);

	//_entityData->Read(guid, guidLength);
	_entityData->Read(position);
	_entityData->Read(destination);
	_entityData->Read(collisionRadius);
	_entityData->Read(inCombat);
	_entityData->Read(currentAttack);

	PlayerData* newData = new PlayerData();

	newData->id = id;//ClientID(guidLength, guid);
	newData->position = position;
	newData->destination = destination;
	newData->collisionRadius = collisionRadius;
	newData->inCombat = inCombat;
	newData->currentAttack = currentAttack;

	//printf("GUID: %s\nPosition: %f, %f, %f\nDestination: %f, %f, %f",
	//	guid, position.x, position.y, position.z,
	//	destination.x, destination.y, destination.z);

	return newData;
}