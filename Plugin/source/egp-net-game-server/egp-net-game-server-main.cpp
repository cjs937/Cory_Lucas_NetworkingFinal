#include "ServerState.h"
#include <iostream>

int main(int const argc, char const *const *const argv)
{
	ServerState* state = new ServerState();

	if (!state->init())
	{
		return -1;
	}

	while (state->shouldLoop())
	{
		state->updateState();
	}

	return 0;
}