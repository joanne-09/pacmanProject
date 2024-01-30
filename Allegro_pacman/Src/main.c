

#include "game.h"
#include<crtdbg.h>
// Program entry point
// Returns program exit code.
int main(int argc, char **argv) 
{
	game_create();
	_CrtDumpMemoryLeaks();
	// printf("Yeah! You just end you game with no error perhaps.")
	return 0;
}
