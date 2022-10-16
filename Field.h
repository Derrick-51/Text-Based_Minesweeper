#ifndef FIELD_H
#define FIELD_H

#include "Zone.h"
#include <vector>

class Field
{
public:
	enum Difficulty
	{
		easy,
		medium,
		hard,

		max_difficulties
	};

	Field(Difficulty difficulty = easy);

	int fieldSizeInit(Difficulty diff);

	void exposeFirstZone(int row, int col);

	void exposeZone(int row, int col);

	void incrementZone(int row, int col);

	void decrementZone(int row, int col);

	void exposeAll();

	friend void print(const Field& field);

	int squareSize() const;

	bool mineExposed() const;

	void flagZone(int row, int col);

	bool isCleared() const;

	void blastZone(int row, int col);

	void usePower(int row, int col);

private:
	std::vector<Zone> m_zone;

	Difficulty m_difficulty{};
	int m_fieldMines{ 0 };
	int m_exposedZones{ 0 };
	int m_fieldSize{};			// Non-const to use jamboree power
	int m_flatFieldSize{};		// Non-const to use jamboree power
	bool m_mineExposed{ false };

	Field(Difficulty diff, int exposedZones, int mines);

	void initialSetup();

	void jamboreeSetup();

	int fieldIndex(int row, int col) const;

	void placeMine(int row, int col);

	// Only works when placed within class declaration
	// Used to distribute functions to multiple zones
	using memberFunc_t = void (Field::*)(int, int);

	void invokeAtAdjacents(int originRow, int originCol, memberFunc_t function);

	void invokeHorizontal(int originRow, int originCol, memberFunc_t function);

	void invokeVertical(int originRow, int originCol, memberFunc_t function);

	void horizontalBlast(int row, int col);

	void verticalBlast(int row, int col);

	void radialBlast(int row, int col);

	void jamboree(int row, int col);

};

#endif