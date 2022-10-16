#include "GameLogic.h"

#include <iostream>
#include <limits>

/*
	Text-Based Minesweeper

	Minesweeper clone with dynamic sized grid.
	May be adapted to add GUI eventually.
*/

bool keepPlaying();

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

bool keepPlaying()
{
	char input{};
	while (true)
	{
		std::cout << "Would you like to play again? (y/n) ";
		std::cin >> input;

		if (std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cerr << "Invalid input, try again.\n\n";
			continue;
		}

		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		switch (input)
		{
		case 'y':
		case 'Y':
			return true;
		case 'n':
		case 'N':
			return false;
		}
	}
}