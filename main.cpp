#include "GameLogic.h"

/*
	Text-Based Minesweeper

	Minesweeper clone with dynamic sized grid.
	May be adapted to add GUI eventually.
*/



int main()
{
	do
	{
		if (playGame())
		{
			std::cout << "\n\nCongratulations, you win!\n\n";
		}
		else
		{
			std::cout << "\n\nYou blew up!\n\n";
		}

	} while (keepPlaying());

	return 0;
} 