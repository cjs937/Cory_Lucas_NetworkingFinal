#include "GUID.h"
#include <string>

GUID::GUID(): guidLength(0), guid(nullptr)
{}

GUID::GUID(int _length, char* _guid) : guidLength(_length), guid(_guid)
{}


GUID::~GUID()
{
	if (guid != nullptr)
		delete[] guid;
}

bool GUID::operator==(GUID const & _compareTo) const
{
	if (strcmp(guid, _compareTo.guid) == 0)
		return true;

	return false;
}