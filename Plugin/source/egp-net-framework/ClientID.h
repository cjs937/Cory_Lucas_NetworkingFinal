#ifndef CLIENT_ID_H
#define CLIENT_ID_H

#pragma once

#pragma pack (push, 1)
struct ClientID
{
	ClientID();
	ClientID(int _length, char* _guid);
	~ClientID();

	int guidLength;
	char* guid;

	bool operator ==(ClientID const &_compareTo) const;
};
#pragma pack(pop)
#endif // !CLIENT_ID_H
