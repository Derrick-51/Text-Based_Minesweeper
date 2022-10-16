#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "Field.h"

#include <string>
#include <string_view>

enum class Action
{
	expose,
	flag,
	power
};

struct InputData
{
	Action action{};
	int row{};
	int col{};
};

Field::Difficulty difficultyPrompt();
bool validSelectionFormat(std::string_view input, std::string_view format);
InputData playerActionSelect(const Field& field, std::string& inputStr);
void performAction(Field& field, InputData input);
bool keepPlaying();
bool playGame();

#endif