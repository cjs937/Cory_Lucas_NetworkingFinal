#include "PlayerData.h"

PlayerData::PlayerData()
{}

PlayerData::PlayerData(GUID _id, Vector2 _position, Vector2 _targetPosition, float _collisionRadius, bool _inCombat, int _currentAttack) : id(_id), position(_position), targetPosition(_targetPosition),
inCombat(_inCombat), currentAttack(_currentAttack)
{}

PlayerData::~PlayerData()
{}
