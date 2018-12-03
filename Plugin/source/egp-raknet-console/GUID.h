#pragma once

struct GUID
{
	GUID();
	GUID(int _length, char* _guid);
	~GUID();

	int guidLength;
	char* guid;

	bool operator ==(GUID const &_compareTo) const;
};
