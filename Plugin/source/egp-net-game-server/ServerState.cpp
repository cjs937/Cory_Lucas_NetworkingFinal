#include "ServerState.h"
#include <iostream>
#include <string>
#include "egp-net-framework/DemoPeerManager.h"
#include "egp-net-framework/PlayerData.h"

ServerState::ServerState()
{
	runLoop = true;
}


ServerState::~ServerState()
{}

void ServerState::updateState()
{

	std::chrono::time_point<std::chrono::system_clock> currentTime = std::chrono::system_clock::now();
	std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> currentTimeMS
		= std::chrono::time_point_cast<std::chrono::milliseconds>(currentTime);

	std::chrono::milliseconds elapsedChronoTime = currentTimeMS - lastTimeMS;
	std::chrono::milliseconds elapsedChronoNetworkTime = currentTimeMS - lastNetworkUpdateMS;

	float elapsedTime = (float)elapsedChronoTime.count();
	float elapsedNetworkTime = (float)elapsedChronoNetworkTime.count();

	if (elapsedNetworkTime > DemoPeerManager::getInstance()->networkTickRateMS) // TODO: Mutex this
	{
		checkWorldCollisions();

		//Clear data list
		std::queue<PlayerData*> unresolvedData; //unresolved means that the data hasn't been used for anything yet

		for (int i = 0; i < dataThisFrame.size(); ++i)
		{
			if (dataThisFrame[i])
			{
				if (!dataThisFrame[i]->unresolved)
					delete dataThisFrame[i];
				else
					unresolvedData.push(dataThisFrame[i]);
			}
			
		}

		dataThisFrame.clear();

		while (unresolvedData.size() > 0)
		{
			dataThisFrame.push_back(unresolvedData.front());
			unresolvedData.pop();
		}

		//Clear attackers list
		std::queue<PlayerData*> unresolvedAttackers;

		for (int i = 0; i < pendingAttackers.size(); ++i)
		{
			if (pendingAttackers[i])
			{
				if (!pendingAttackers[i]->unresolved)
					delete pendingAttackers[i];
				else
					unresolvedAttackers.push(pendingAttackers[i]);
			}

		}

		pendingAttackers.clear();

		while (unresolvedAttackers.size() > 0)
		{
			pendingAttackers.push_back(unresolvedAttackers.front());
			unresolvedAttackers.pop();
		}

		lastNetworkUpdateMS = currentTimeMS;
	}

	lastTimeMS = currentTimeMS;
}

void ServerState::exitLoop()
{
	runLoop = false;
}

bool ServerState::shouldLoop()
{
	return runLoop;
}

bool ServerState::init()
{

	int numClients;
	int port = DemoPeerManager::getInstance()->serverPort;
	std::cout << "Enter maximum number of clients: \n";

	std::cin >> numClients;

	if (!DemoPeerManager::getInstance()->StartupNetworking(false, numClients, port, true))
	{
		return false;
	}

	//Timing
	lastTime = std::chrono::system_clock::now();
	lastTimeMS = std::chrono::time_point_cast<std::chrono::milliseconds>(lastTime);
	lastNetworkUpdateMS = std::chrono::time_point_cast<std::chrono::milliseconds>(lastTime);

	PlayerData* d1 = new PlayerData();
	d1->collisionRadius = 5;
	d1->position = Vector3(1, 3);
	d1->id.guid = "dfdf";
	PlayerData* d2 = new PlayerData();
	d2->collisionRadius = 1;
	d1->position = Vector3(1, 7);
	d2->id.guid = "aaaa";

	addPlayerData(d1);
	addPlayerData(d2);

	return true;

}

void ServerState::checkWorldCollisions()
{
	for (int i = 0; i < dataThisFrame.size(); ++i)
	{
		PlayerData* currentPlayer = dataThisFrame[i];
		
		if (currentPlayer->inCombat)
			continue;

		for (int j = i + 1; j < dataThisFrame.size(); ++j)
		{
			PlayerData* thisPlayer = dataThisFrame[j];

			if (thisPlayer->inCombat || currentPlayer->id == thisPlayer->id)
				continue;
			
			if (Vector3::Distance(currentPlayer->position, thisPlayer->position) <= currentPlayer->collisionRadius)
			{
				std::cout << "works\n";
				
				sendCollisionPacket(currentPlayer->id, thisPlayer->id);

				currentPlayer->inCombat = true;
				thisPlayer->inCombat = true;

				//Collision found call send
				break;
			}

			thisPlayer->unresolved = false;
		}

		std::cout << "donecheckin\n";
	}
}

void ServerState::sendCollisionPacket(ClientID _p1, ClientID _p2)
{
	RakNet::BitStream stream;

	stream.Write((RakNet::MessageID)DemoPeerManager::PLAYER_COLLISION);
	stream.Write(_p1);
	stream.Write(_p2);

	DemoPeerManager::getInstance()->SendPacket(&stream, -1, true, true);
}

bool ServerState::canPerformAttacks(PlayerData* _player, ClientID _opponentID)
{
	for (int i = 0; i < pendingAttackers.size(); ++i)
	{
		if (pendingAttackers[i]->id == _opponentID && pendingAttackers[i]->unresolved)
		{
			handlePlayerBattle(_player, pendingAttackers[i]);

			pendingAttackers[i]->unresolved = false;

			return true;
		}
	}

	return false;
}

void ServerState::handlePlayerBattle(PlayerData* _player1, PlayerData* _player2) // TODO
{
	// 0 = rock, 1 = scissors, 2 = paper

	if (_player1->currentAttack == 0) // rock
	{
		if (_player2->currentAttack == 0)
		{
			//draw
		}
		else if (_player2->currentAttack == 1)
		{
			//p1 win
		}
		else
		{
			//p2 win
		}
	}
	else if (_player1->currentAttack == 1) //paper
	{
		if (_player2->currentAttack == 0)
		{
			//p1 win
		}
		else if (_player2->currentAttack == 1)
		{
			//draw
		}
		else
		{
			//p2 win
		}
	}
	else // scissors
	{
		if (_player2->currentAttack == 0)
		{
			//p2 win
		}
		else if (_player2->currentAttack == 1)
		{
			//p1 win
		}
		else
		{
			//draw
		}
	}
}

void ServerState::addPlayerData(PlayerData* _data)
{
	dataThisFrame.push_back(_data);
}

void ServerState::addPlayerData(RakNet::Packet* _entityPacket) const
{
	addPlayerData(createPlayerFromPacket(_entityPacket));
}

//TODO: Call this from networking loop
void ServerState::handleAttacker(RakNet::Packet* _entityData, ClientID _opponentID)
{
	PlayerData* player = createPlayerFromPacket(_entityData);

	if (canPerformAttacks(player, _opponentID)) // fires attack/win event
		delete player;
	else
	{
		pendingAttackers.push_back(player);
	}
}

PlayerData* ServerState::createPlayerFromPacket(RakNet::Packet* _entityPacket)
{
	RakNet::BitStream stream;
	int guidLength;
	Vector3 position;
	Vector3 destination;
	float collisionRadius;
	bool inCombat;
	int currentAttack;

	stream.IgnoreBytes(sizeof((char)DemoPeerManager::UPDATE_NETWORK_PLAYER));

	stream.Read(guidLength);
	char* guid = new char[guidLength];

	stream.Read(guid, guidLength);
	stream.Read(position);
	stream.Read(destination);
	stream.Read(collisionRadius);
	stream.Read(inCombat);
	stream.Read(currentAttack);

	PlayerData* newData = new PlayerData();

	newData->id = ClientID(guidLength, guid);
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


ServerState* ServerState::getInstance()
{
	static ServerState instance;

	return &instance;
}