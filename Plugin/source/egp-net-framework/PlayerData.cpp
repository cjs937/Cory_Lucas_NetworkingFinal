#include "PlayerData.h"

PlayerData::PlayerData()
{}

PlayerData::PlayerData(ClientID _id, Vector3 _position, Vector3 _targetPosition, float _collisionRadius, bool _inCombat, int _currentAttack) : id(_id), position(_position), targetPosition(_targetPosition),
inCombat(_inCombat), currentAttack(_currentAttack)
{}

PlayerData::~PlayerData()
{}
