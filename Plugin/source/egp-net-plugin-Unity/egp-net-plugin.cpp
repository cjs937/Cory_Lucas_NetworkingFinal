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
	char* opponentGuid;
	int action;
};

struct CollisionUpdate
{
	int guidLength1;
	int guidLength2;
	char* guid1;
	char* guid2;

};

struct RoundWinUpdate
{
	int winnerGuidLength;
	int loserGuidLength;
	char* winnerGuid;
	char* loserGuid;
	bool draw;

};

// create and return instance of peer interface
RakNet::RakPeerInterface *peer;

DemoPeerManager* peerManager;

std::queue<EntityUpdate*>* entityUpdates;
std::queue<CollisionUpdate*>* collisionUpdates;
std::queue<RoundWinUpdate*>* roundWinUpdates;

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
		collisionUpdates = new std::queue<CollisionUpdate*>();
		roundWinUpdates = new std::queue<RoundWinUpdate*>();

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
	bool getNextCollisionUpdate(int* guid1Length, char** guid1, int* guid2Length, char** guid2)
	{
		if (collisionUpdates->size() == 0)
			return false;

		CollisionUpdate* currUpdate = collisionUpdates->front();
		collisionUpdates->pop();

		*guid1Length = currUpdate->guidLength1;
		*guid1 = currUpdate->guid1;
		*guid2Length = currUpdate->guidLength2;
		*guid2 = currUpdate->guid2;

		return true;
	}

	__declspec(dllexport)	// tmp linker flag, forces lib to exist
	bool getNextRoundWinUpdate(int* winnerGuidLength, char** winnerGuid, int* loserGuidLength, char** loserGuid, bool* draw)
	{
		if (roundWinUpdates->size() == 0)
			return false;

		RoundWinUpdate* currUpdate = roundWinUpdates->front();
		roundWinUpdates->pop();

		*winnerGuidLength = currUpdate->winnerGuidLength;
		*winnerGuid = currUpdate->winnerGuid;
		*loserGuidLength = currUpdate->loserGuidLength;
		*loserGuid = currUpdate->loserGuid;
		*draw = currUpdate->draw;

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
		case DemoPeerManager::PLAYER_COLLISION:
			{
				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data, packet->length, true);

				// Skip the message ID
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

				CollisionUpdate* update = new CollisionUpdate();

				// Get the GUID length and allocate that much space
				// Player 1
				bsIn.Read(update->guidLength1);
				update->guid1 = (char*)malloc(update->guidLength1);

				for (int i = 0; i < update->guidLength1; i++)
				{
					bsIn.Read(update->guid1[i]);
				}
				update->guid1[update->guidLength1] = '\0';

				// Player 2
				bsIn.Read(update->guidLength2);
				update->guid2 = (char*)malloc(update->guidLength2);

				for (int i = 0; i < update->guidLength2 + 1; i++)
				{
					bsIn.Read(update->guid2[i]);
				}
				update->guid2[update->guidLength2] = '\0';

				collisionUpdates->push(update);

				return (int)DemoPeerManager::PLAYER_COLLISION;
			}
			break;
		case DemoPeerManager::PLAYER_WIN_ROUND:
		{
			RakNet::RakString rs;
			RakNet::BitStream bsIn(packet->data, packet->length, true);

			// Skip the message ID
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

			RoundWinUpdate* update = new RoundWinUpdate();

			// Get the GUID length and allocate that much space
			// Player 1
			bsIn.Read(update->winnerGuidLength);
			update->winnerGuid = (char*)malloc(update->winnerGuidLength);

			for (int i = 0; i < update->winnerGuidLength; i++)
			{
				bsIn.Read(update->winnerGuid[i]);
			}
			update->winnerGuid[update->winnerGuidLength];

			// Player 2
			bsIn.Read(update->loserGuidLength);
			update->loserGuid = (char*)malloc(update->loserGuidLength);

			for (int i = 0; i < update->loserGuidLength; i++)
			{
				bsIn.Read(update->loserGuid[i]);
			}
			update->loserGuid[update->loserGuidLength];

			bsIn.Read(update->draw);

			roundWinUpdates->push(update);

			return (int)DemoPeerManager::PLAYER_WIN_ROUND;
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
	bool sendEntityToServer(int guidLength, char* guid, Vector3 position, Vector3 destination, float collisionRadius, bool inCombat, int currentAttack)
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
	bool sendCombatUpdateToServer(int guidLength, char* guid, Vector3 position, Vector3 destination, float collisionRadius, bool inCombat, int currentAttack, int opponentGuidLength, char* opponentGuid)
	{
		RakNet::BitStream bsOut;
		bsOut.Write((RakNet::MessageID)DemoPeerManager::UPDATE_COMBAT_PLAYER);

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

		bsOut.Write(opponentGuidLength);

		for (int i = 0; i < opponentGuidLength; i++)
		{
			bsOut.Write(opponentGuid[i]);
		}

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
