#include "Vector2.h"

Vector2::Vector2(float _x, float _y)
{
	x = _x;
	y = _y;
}

bool Vector2::operator ==(Vector2 const &_compareTo) const
{
	return (_compareTo.x == x && _compareTo.y == y);
}

Vector2 Vector2::operator +(Vector2 const &_right) const
{
	return Vector2(x + _right.x, y + _right.y);
}

Vector2 Vector2::operator -(Vector2 const &_right) const
{
	return Vector2(x - _right.x, y - _right.y);
}

void Vector2::operator +=(Vector2 const &_right)
{
	x += _right.x;
	y += _right.y;
}

void Vector2::operator -=(Vector2 const &_right)
{
	x -= _right.x;
	y -= _right.y;
}

Vector2 Vector2::operator*(float const & _right) const
{
	return Vector2(x * _right, y * _right);
}

//float Vector2::Distance(Vector2 const &_left, Vector2 const &_right)
//{
//	float x = _right.x - _left.x;
//	x *= x;
//	float y = _right.y - _left.y;
//	y *= y;
//
//	return (float)sqrt(x + y);
//}
