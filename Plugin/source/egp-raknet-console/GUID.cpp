#include "GUID.h"
#include <string>

ClientID::ClientID(): guidLength(0), guid(nullptr)
{}

ClientID::ClientID(int _length, char* _guid) : guidLength(_length), guid(_guid)
{}


ClientID::~ClientID()
{
	if (guid != nullptr)
		delete[] guid;
}

bool ClientID::operator==(ClientID const & _compareTo) const
{
	if (strcmp(guid, _compareTo.guid) == 0)
		return true;

	return false;
}