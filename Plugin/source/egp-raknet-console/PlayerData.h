#pragma once
#include "Vector2.h"
#include "GUID.h"

struct PlayerData
{
public:
	PlayerData();
	PlayerData(ClientID _id, Vector2 _position, Vector2 _targetPosition, float _collisionRadius, bool inCombat, int currentAttack = -1);
	~PlayerData();

	ClientID id;
	Vector2 position;
	Vector2 targetPosition;
	float collisionRadius;
	bool inCombat;
	int currentAttack;
};

