 /*
	EGP Networking: Plugin Interface/Wrapper
	Dan Buckstein
	October 2018

	Main implementation of Unity plugin wrapper.
*/

#include "egp-net-plugin.h"

// RakNet includes
#include "RakNet/RakPeerInterface.h"
#include "RakNet/MessageIdentifiers.h"
#include "RakNet/RakNetTypes.h"
#include "RakNet/BitStream.h"

#include "egp-net-framework/DemoPeerManager.h"
#include "egp-net-framework/Vector3.h"

#pragma pack(push, 1)

struct GameMessageData
{
	unsigned char typeID;

	// ****TO-DO: implement game message data struct

};

#pragma pack (pop)

struct EntityUpdate
{
	int guidLength;
	char* guid;
	Vector3 position;
	Vector3 destination;
	RakNet::Time latency;
};

struct CombatUpdate
{
	int guidLength;
	char* opponetGuid;
	int action;
};

// create and return instance of peer interface
RakNet::RakPeerInterface *peer;

DemoPeerManager* peerManager;

std::queue<EntityUpdate*>* entityUpdates;
std::queue<CombatUpdate*>* combatUpdates;

extern "C"
{
	// dummy function for testing
	__declspec(dllexport)	// tmp linker flag, forces lib to exist
	int foo(int bar)
	{
		return (bar + 1);
	}

	__declspec(dllexport)	// tmp linker flag, forces lib to exist
	bool initNetworking(char* ip)
	{
		peerManager = DemoPeerManager::getInstance();

		if (peerManager->StartupNetworking(true, 0, 0, false) == 0)
		{
			return false;
		}

		peerManager->serverAddress = ip;
		if (peerManager->Connect(ip, peerManager->serverPort) <= 0)
		{
			return false;
		}

		peer = peerManager->getPeer();

		entityUpdates = new std::queue<EntityUpdate*>();
		combatUpdates = new std::queue<CombatUpdate*>();

		return true;

	}

	__declspec(dllexport)	// tmp linker flag, forces lib to exist
	bool getNextEntityUpdate(int* guidLength, char** guid, Vector3* position, Vector3* destination, RakNet::Time* latency)
	{
		if (entityUpdates->size() == 0)
			return false;

		EntityUpdate* currUpdate = entityUpdates->front();
		entityUpdates->pop();

		*guidLength = currUpdate->guidLength;
		*guid = currUpdate->guid;
		*position = currUpdate->position;
		*destination = currUpdate->destination;
		*latency = currUpdate->latency;

		return true;
	}

	__declspec(dllexport)	// tmp linker flag, forces lib to exist
	bool getNextCombatUpdate(int* guidLength, char** opponetGuid, int* action)
	{
		if (combatUpdates->size() == 0)
			return false;

		CombatUpdate* currUpdate = combatUpdates->front();
		combatUpdates->pop();

		*guidLength = currUpdate->guidLength;
		*opponetGuid = currUpdate->opponetGuid;
		*action = currUpdate->action;

		return true;
	}

	__declspec(dllexport)	// tmp linker flag, forces lib to exist
	int handlePacket()
	{
		RakNet::Packet* packet;

		packet = peer->Receive();
		if (!packet)
		{
			// No packet
			return -1;
		}
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
			//if (isServer) {
				//printf("A client has disconnected.\n");
			//}
			//else {
				printf("We have been disconnected.\n");
			//}
			break;
		case ID_CONNECTION_LOST:
			//if (isServer) {
			//	printf("A client lost the connection.\n");
			//}
			//else {
				printf("Connection lost.\n");
			//}
			//break;
		
		case DemoPeerManager::ID_GAME_MESSAGE_1:
			printf("DEBUG MESSAGE: received packet ID_GAME_MESSAGE_1.\n");
			{
				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(rs);
				//printf("%s\n", rs.C_String());
		
				char* returnThis = (char*)rs.C_String();
		
				return (int) DemoPeerManager::ID_GAME_MESSAGE_1;
		
				// ****TO-DO: read packet without using bitstream
		
			}
			break;
		
		case DemoPeerManager::UPDATE_NETWORK_PLAYER:
			printf("DEBUG MESSAGE: received packet ID_GAME_MESSAGE_1.\n");
			{
				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data, packet->length, true);
				
				// Skip the message ID
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

				EntityUpdate* update = new EntityUpdate();

				// Get the time difference between the client and server
				RakNet::Time dt, ourt, theirt;

				ourt = RakNet::GetTime();
				bsIn.Read(theirt);
				dt = ourt - theirt;

				update->latency = dt;

				// Get the GUID length and allocate that much space
				bsIn.Read(update->guidLength);
				update->guid = (char*)malloc(update->guidLength);

				for (int i = 0; i < update->guidLength; i++)
				{
					bsIn.Read(update->guid[i]);
				}

				bsIn.Read(update->position);
				bsIn.Read(update->destination);

				entityUpdates->push(update);

				return (int)DemoPeerManager::UPDATE_NETWORK_PLAYER;
				
				// Construct a new stream containing the info we recieved and the time
				//RakNet::BitStream bsOut;
				//bsOut.Write((RakNet::MessageID)DemoPeerManager::UPDATE_NETWORK_PLAYER);
				//bsOut.Write(dt);
				//bsOut.Write(bsIn);

				// Convert the stream to a char* for returning to C#
				//int byteCount = (int) bsOut.GetNumberOfBytesUsed();
				//char* retData = (char*)malloc(byteCount);
				//
				//bsOut.Read(retData, byteCount);
				//
				//// Set the length and return the data
				//*length = (int)byteCount;
				//return retData;
			}
			break;
		
		default:
			printf("Message with identifier %i has arrived.\n", packet->data[0]);
			break;
		}

		peer->DeallocatePacket(packet);

		// Unknown packet
		return packet->data[0];
	}

	__declspec(dllexport)	// tmp linker flag, forces lib to exist
	char* plsreturn(int* length)
	{
		char* arrayTest = "yeeting";
		*length = sizeof("yeeting");
		return arrayTest;
	}

	__declspec(dllexport)	// tmp linker flag, forces lib to exist
	bool sendEntityToServer(int guidLength, char* guid, Vector3 position, Vector3 destination, float collisionRadius, bool inCombat = false, int currentAttack = 0)
	{
		RakNet::BitStream bsOut;
		bsOut.Write((RakNet::MessageID)DemoPeerManager::UPDATE_NETWORK_PLAYER);

		const RakNet::Time timeSend = RakNet::GetTime();
		bsOut.Write(timeSend);

		bsOut.Write(guidLength);

		for (int i = 0; i < guidLength; i++)
		{
			bsOut.Write(guid[i]);
		}

		bsOut.Write(position);
		bsOut.Write(destination);
		bsOut.Write(collisionRadius);
		bsOut.Write(inCombat);
		bsOut.Write(currentAttack);

		peerManager->sendEntity(&bsOut);

		return true;
	}

	__declspec(dllexport)	// tmp linker flag, forces lib to exist
	bool disconnect()
	{
		if (peerManager->Disconnect() != 0)
			return true;

		return false;

	}

	__declspec(dllexport)	// tmp linker flag, forces lib to exist
	bool shutdownNetworking()
	{
		if (peerManager->ShutdownNetworking() != 0)
			return true;

		return false;

	}
}
