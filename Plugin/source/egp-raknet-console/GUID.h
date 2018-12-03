#pragma once

struct ClientID
{
	ClientID();
	ClientID(int _length, char* _guid);
	~ClientID();

	int guidLength;
	char* guid;

	bool operator ==(ClientID const &_compareTo) const;
};
