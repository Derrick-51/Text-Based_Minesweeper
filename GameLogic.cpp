#include "GameLogic.h"
#include "Field.h"
#include <iostream>
#include <string>
#include <limits>
#include <algorithm>
#include <map>

// Returns difficulty
Field::Difficulty difficultyPrompt()
{
	using enum Field::Difficulty;

	char difficulty{};
	while(true)
	{
		std::cout << "Select Difficulty (E)asy, (M)edium, or (H)ard: ";
		std::cin >> difficulty;

		// Clear stream to prevent future unwanted input 
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		switch (difficulty)
		{
		case 'e':
		case 'E':
			return easy;
		case 'm':
		case 'M':
			return medium;
		case 'h':
		case 'H':
			return hard;
		default:
			std::cerr << "Invalid input, try again.\n\n";
			continue;
		}
	}
}

// returns true if string matches given format
bool validSelectionFormat(std::string_view input, std::string_view format)
{
	if (input.length() != format.length())
		return false;

	static const std::map<char, int (*)(int)> validators
	{
		{'@', std::isalpha},
		{'_', std::isspace},
		{'#', std::isdigit},
		{'?', [](int) { return 1; }}
	};

	// checks that each char in string matches corresponding validator
	return std::ranges::equal(input, format, [](char ch, char mask)->bool
		{
			if (auto found{ validators.find(mask) }; found != validators.end())
			{
				return found->second(ch);
			}
			else
			{
				return ch == mask;
			}
		});
}

InputData playerActionSelect(const Field& field, std::string& inputStr)
{
	// To convert columns from char A-Z to int value (e.g. A = 0)
	constexpr int asciiUpAlphaOffset{ 65 };

	// To convert columns from char a-z to int value (e.g. a = 0)
	constexpr int asciiLowAlphaOffset{ 97 };

	// Holds upper/lower based on condition
	int adjustedOffset{};

	// Index of string where row starts
	constexpr int rowStrIdx{ 2 };

	InputData input{};

	while(true)
	{
		std::cout << "Actions:\n(E)xpose zone (F)lag zone (P)ower use\n\n"
				  << "Select Action (Ex. f 2a): ";

		std::getline(std::cin, inputStr);

		if (!validSelectionFormat(inputStr, "@_#@")
			&& !validSelectionFormat(inputStr, "@_##@"))	// Example input: "E 2C" to expose zone 2C
			continue;

		// If final char (column letter) is uppercase set upper offset
		adjustedOffset = std::isupper(inputStr[inputStr.length() - 1]) ?
							asciiUpAlphaOffset : asciiLowAlphaOffset;

		input.row = std::stoi(inputStr.substr(rowStrIdx)) - 1;	// Start at row index with conversion

		input.col = inputStr[inputStr.length() - 1] - adjustedOffset;	// Column is final char (upper/lower case)


		if ((input.row >= field.squareSize()) || (input.row < 0)
			|| (input.col >= field.squareSize()) || (input.col < 0))
		{
			std::cerr << "Zone out of bounds.\n\n";
			continue;
		}
		
		switch (inputStr[0])
		{
		case 'e':
		case 'E':
			input.action = Action::expose;
			return input;
		case 'f':
		case 'F':
			input.action = Action::flag;
			return input;
		case 'p':
		case 'P':
			input.action = Action::power;
			return input;
		default:
			std::cerr << "Invalid Action.\n\n";
			break;
		}
	}
}

void performAction(Field& field, InputData input)
{
	static bool firstAction{ true };

	switch (input.action)
	{
	case Action::expose:
		if (firstAction)
		{
			field.exposeFirstZone(input.row, input.col);
			firstAction = false;
		}
		else
		{
			field.exposeZone(input.row, input.col);
		}
		break;
	case Action::flag:
		field.flagZone(input.row, input.col);
		break;
	case Action::power:
		if (firstAction)
		{
			field.usePower(input.row, input.col);
			firstAction = false;
		}
		else
		{
			field.usePower(input.row, input.col);
		}
		break;
	default:
		std::cerr << "Invalid Action.\n\n";
		break;
	}
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

bool playGame()
{
	std::cout << "Welcome to Text-Based Minesweeper\n";

	Field field{difficultyPrompt()};

	std::string userInputStr{};
	InputData input{};


	print(field);

	do
	{
		input = playerActionSelect(field, userInputStr);

		performAction(field, input);

		print(field);
	} while (!field.isCleared() && !field.mineExposed());

	if (field.mineExposed())
		return false;
	else
		return true;
}