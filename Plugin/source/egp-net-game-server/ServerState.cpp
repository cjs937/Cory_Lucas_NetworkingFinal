#include "ServerState.h"
#include <iostream>
#include <string>
#include "egp-net-framework/DemoPeerManager.h"
#include "egp-net-framework/PlayerData.h"

ServerState::ServerState()
{
	runLoop = true;

	dataThisFrame = &(DemoPeerManager::getInstance()->pendingPlayerUpdates);
	pendingAttackers = &(DemoPeerManager::getInstance()->pendingAttackers);
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

		for (int i = 0; i < dataThisFrame->size(); ++i)
		{
			if ((*dataThisFrame)[i])
			{
				if (!(*dataThisFrame)[i]->unresolved)
					delete (*dataThisFrame)[i];
				else
					unresolvedData.push((*dataThisFrame)[i]);
			}
			
		}

		(*dataThisFrame).clear();

		while (unresolvedData.size() > 0)
		{
			(*dataThisFrame).push_back(unresolvedData.front());
			unresolvedData.pop();
		}

		//Clear attackers list
		std::queue<PlayerData*> unresolvedAttackers;

		for (int i = 0; i < pendingAttackers->size(); ++i)
		{
			if ((*pendingAttackers)[i])
			{
				if (!(*pendingAttackers)[i]->unresolved)
					delete (*pendingAttackers)[i];
				else
					unresolvedAttackers.push((*pendingAttackers)[i]);
			}

		}

		(*pendingAttackers).clear();

		while (unresolvedAttackers.size() > 0)
		{
			(*pendingAttackers).push_back(unresolvedAttackers.front());
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
	for (int i = 0; i < dataThisFrame->size(); ++i)
	{
		PlayerData* currentPlayer = (*dataThisFrame)[i];
		
		if (currentPlayer->inCombat)
			continue;

		for (int j = i + 1; j < dataThisFrame->size(); ++j)
		{
			PlayerData* thisPlayer = (*dataThisFrame)[j];

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

	stream.Write(_p1.guidLength);
	for (int i = 0; i < _p1.guidLength; i++)
	{
		stream.Write(_p1.guid[i]);
	}

	stream.Write(_p2.guidLength);
	for (int i = 0; i < _p2.guidLength; i++)
	{
		stream.Write(_p2.guid[i]);
	}

	DemoPeerManager::getInstance()->SendPacket(&stream, -1, true, true);
}

bool ServerState::canPerformAttacks(PlayerData* _player, ClientID _opponentID)
{
	for (int i = 0; i < pendingAttackers->size(); ++i)
	{
		for (int j = i + 1; j < pendingAttackers->size(); ++j
		{

		}
		//if ((*pendingAttackers)[i]->id == _opponentID && (*pendingAttackers)[i]->unresolved)
		//{
		//	handlePlayerBattle(_player, (*pendingAttackers)[i]);

		//	(*pendingAttackers)[i]->unresolved = false;

		//	return true;
		//}
	}

	return false;
}

void ServerState::handlePlayerBattle(PlayerData* _player1, PlayerData* _player2) // TODO
{
	// 0 = rock, 1 = scissors, 2 = paper
	ClientID winner;
	ClientID loser;
	bool isDraw = false;

	bool p1Win = false;
	bool p2Win = false;

	if (_player1->currentAttack == 0) // rock
	{
		if (_player2->currentAttack == 0)
		{
			//draw	
		}
		else if (_player2->currentAttack == 1)
		{
			p1Win = true;
		}
		else
		{
			p2Win = true;
		}
	}
	else if (_player1->currentAttack == 1) //paper
	{
		if (_player2->currentAttack == 0)
		{
			p1Win = true;
		}
		else if (_player2->currentAttack == 1)
		{
			//draw
		}
		else
		{
			p2Win = true;
		}
	}
	else // scissors
	{
		if (_player2->currentAttack == 0)
		{
			p2Win = true;
		}
		else if (_player2->currentAttack == 1)
		{
			p1Win = true;
		}
		else
		{
			//draw
		}
	}

	if (p1Win)
	{
		winner = _player1->id;
		loser = _player2->id;
	}
	else if (p2Win)
	{
		winner = _player2->id;
		loser = _player1->id;
	}
	else
	{
		winner = _player1->id;
		loser = _player2->id;

		isDraw = true;
	}

	sendRoundWinnerPacket(winner, loser, isDraw);
}

void ServerState::sendRoundWinnerPacket(ClientID _winner, ClientID _loser, bool isDraw)
{
	RakNet::BitStream stream;
	stream.Write((RakNet::MessageID)DemoPeerManager::PLAYER_WIN_ROUND);
	stream.Write(_winner.guidLength);
	for (int i = 0; i < _winner.guidLength; i++)
	{
		stream.Write(_winner.guid[i]);
	}

	stream.Write(_loser.guidLength);
	for (int i = 0; i < _loser.guidLength; i++)
	{
		stream.Write(_loser.guid[i]);
	}
	stream.Write(isDraw);

	DemoPeerManager::getInstance()->SendPacket(&stream, -1, true, true);
}


//void ServerState::addPlayerData(PlayerData* _data)
//{
//	(*dataThisFrame).push_back(_data);
//}


//void ServerState::addPlayerData(RakNet::BitStream* _entityData)
//{
//	addPlayerData(createPlayerFromPacket(_entityData));
//}

//TODO: Call this from networking loop
void ServerState::handleAttacker(RakNet::BitStream* _entityData, ClientID _opponentID)
{
	PlayerData* player = createPlayerFromPacket(_entityData);

	if (canPerformAttacks(player, _opponentID)) // fires attack/win event
		delete player;
	else
	{
		(*pendingAttackers).push_back(player);
	}
}

//PlayerData* ServerState::createPlayerFromPacket(RakNet::BitStream* _entityData)
//{
//	int guidLength;
//	Vector3 position;
//	Vector3 destination;
//	float collisionRadius;
//	bool inCombat;
//	int currentAttack;
//
//	//_entityData.IgnoreBytes(sizeof((char)DemoPeerManager::UPDATE_NETWORK_PLAYER));
//
//	_entityData->Read(guidLength);
//	char* guid = new char[guidLength];
//
//	_entityData->Read(guid, guidLength);
//	_entityData->Read(position);
//	_entityData->Read(destination);
//	_entityData->Read(collisionRadius);
//	_entityData->Read(inCombat);
//	_entityData->Read(currentAttack);
//
//	PlayerData* newData = new PlayerData();
//
//	newData->id = ClientID(guidLength, guid);
//	newData->position = position;
//	newData->destination = destination;
//	newData->collisionRadius = collisionRadius;
//	newData->inCombat = inCombat;
//	newData->currentAttack = currentAttack;
//
//	//printf("GUID: %s\nPosition: %f, %f, %f\nDestination: %f, %f, %f",
//	//	guid, position.x, position.y, position.z,
//	//	destination.x, destination.y, destination.z);
//
//	return newData;
//}


ServerState* ServerState::getInstance()
{
	static ServerState instance;

	return &instance;
}

void ServerState::shutdownServer()
{
	RakNet::BitStream stream;

	stream.Write((RakNet::MessageID)DemoPeerManager::SERVER_SHUTDOWN);

	DemoPeerManager::getInstance()->SendPacket(&stream, -1, true, true);
}