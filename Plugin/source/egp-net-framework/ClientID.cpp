#include "ClientID.h"
#include <string>

ClientID::ClientID(): guidLength(0), guid(nullptr)
{}

ClientID::ClientID(RakNet::BitStream* bs)
{
	bs->Read(guidLength);

	guid = (char*)malloc(guidLength);

	for (int i = 0; i < guidLength; i++)
	{
		bs->Read(guid[i]);
	}
};

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