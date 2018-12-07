#ifndef PLAYER_DATA_H
#define PLAYER_DATA_H

#pragma once
#include "../egp-net-framework/Vector3.h"
#include "ClientID.h"

struct PlayerData
{
public:
	PlayerData();
	PlayerData(ClientID _id, Vector3 _position, Vector3 _targetPosition, float _collisionRadius, bool inCombat, int currentAttack = -1);
	~PlayerData();

	bool unresolved;

	ClientID id;
	Vector3 position;
	Vector3 destination;
	float collisionRadius;
	bool inCombat;
	int numLives;
	int currentAttack;
};

#endif
