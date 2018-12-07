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

int DemoPeerManager::ProcessPacket(const RakNet::Packet *const packet, const unsigned int packetIndex) const
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
	case ID_CONNECTION_REQUEST_ACCEPTED:
		printf("Our connection request has been accepted.\n");
		{
			// Use a BitStream to write a custom user message
			// Bitstreams are easier to use than sending casted structures, 
			//	and handle endian swapping automatically
			//RakNet::BitStream bsOut;
			//bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
			//bsOut.Write("Hello world");
			//peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

			// ****TO-DO: write and send packet without using bitstream

		}
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
		
		ServerState::getInstance()->addPlayerData(packet);

		std::cout << "Done" << std::endl;
	}
		break;
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


DemoPeerManager::DemoPeerManager()
{
}

DemoPeerManager::~DemoPeerManager()
{
	// ****TO-DO
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