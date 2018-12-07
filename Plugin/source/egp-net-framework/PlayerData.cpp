#include "PlayerData.h"

PlayerData::PlayerData()
{
	unresolved = true;
}

PlayerData::PlayerData(ClientID _id, Vector3 _position, Vector3 _targetPosition, float _collisionRadius, bool _inCombat, int _currentAttack) : id(_id), position(_position), destination(_targetPosition),
inCombat(_inCombat), currentAttack(_currentAttack)
{
	unresolved = true;
}

PlayerData::~PlayerData()
{}
