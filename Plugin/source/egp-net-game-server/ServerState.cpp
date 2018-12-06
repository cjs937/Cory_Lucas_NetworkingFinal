#include "ServerState.h"
#include <iostream>
#include <string>
#include "egp-net-framework/DemoPeerManager.h"
//#include "egp-raknet-console/ClientID.h"

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

	if (elapsedNetworkTime > DemoPeerManager::getInstance()->networkTickRateMS)
	{
		//printf("Sending state to players\n");
		//broadcastState();

		checkWorldCollisions();
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

		for (int j = i + 1; j < dataThisFrame.size(); ++j)
		{
			PlayerData* thisPlayer = dataThisFrame[j];

			if (currentPlayer->id == thisPlayer->id)
				continue;
			
			if (Vector3::Distance(currentPlayer->position, thisPlayer->position) <= currentPlayer->collisionRadius)
			{
				std::cout << "works\n";
				//Collision found
				continue;
			}
		}

		std::cout << "donecheckin\n";
	}
}

void ServerState::addPlayerData(PlayerData* _data)
{
	dataThisFrame.push_back(_data);
}


ServerState* ServerState::getInstance()
{
	static ServerState instance;

	return &instance;
}