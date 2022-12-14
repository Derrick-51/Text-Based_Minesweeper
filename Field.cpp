#include "Field.h"

#include "RNG.h"

#include <random>
#include <iostream>
#include <iomanip>
#include <utility>
#include <functional>

// Length of square sides
static constexpr int g_easyFieldSize{ 8 };
static constexpr int g_mediumFieldSize{ 10 };
static constexpr int g_hardFieldSize{ 12 };

// Mine ranges for difficulties
static constexpr int g_easyMinMines{ 8 };
static constexpr int g_easyMaxMines{ 12 };
static constexpr int g_mediumMinMines{ 16 };
static constexpr int g_mediumMaxMines{ 21 };
static constexpr int g_hardMinMines{ 30 };
static constexpr int g_hardMaxMines{ 40 };

// Used to distribute functions to multiple zones
using memberFunc_t = std::function<void(int,int)>;

// Basic randomized constructor
Field::Field(Difficulty diff)
	:m_difficulty{diff}
	,m_fieldSize{fieldSizeInit(diff)}
	,m_flatFieldSize{ fieldSizeInit(diff) * fieldSizeInit(diff) }
	,m_zone(fieldSizeInit(diff) * fieldSizeInit(diff))
{
	initialSetup();
}

// ** Exclusively used with jamboree power **
// Randomly distributes mines to equal given mine count
Field::Field(Difficulty diff, int exposedZones, int mines)
	:m_difficulty{ diff }
	, m_fieldSize{ fieldSizeInit(diff) }
	, m_flatFieldSize{ fieldSizeInit(diff) * fieldSizeInit(diff) }
	, m_zone(fieldSizeInit(diff) * fieldSizeInit(diff))
	, m_exposedZones{ exposedZones }
	, m_fieldMines{ mines }
{
	jamboreeSetup();
}

void Field::initialSetup()
{
	// Total amount of mines on field
	int mineCount{};
	switch (m_difficulty)
	{
	case easy:		mineCount = RNG::generateRandInt(g_easyMinMines, g_easyMaxMines);		break;
	case medium:	mineCount = RNG::generateRandInt(g_mediumMinMines, g_mediumMaxMines);	break;
	case hard:		mineCount = RNG::generateRandInt(g_hardMinMines, g_hardMaxMines);		break;
	default:		mineCount = RNG::generateRandInt(g_hardMinMines, g_hardMaxMines);		break;
	}

	std::uniform_int_distribution rollZone{ 0, m_fieldSize - 1 };

	int randRow{};
	int randCol{};
	while (mineCount > 0)
	{
		randRow = rollZone(RNG::mt);
		randCol = rollZone(RNG::mt);

		if (m_zone[fieldIndex(randRow, randCol)].hasMine())
		{
			continue;
		}

		placeMine(randRow, randCol);
		--mineCount;
	}
}

// Randomly distributes mines to equal current mine count
void Field::jamboreeSetup()
{
	std::uniform_int_distribution rollZone{ 0, m_fieldSize - 1 };

	int randRow{};
	int randCol{};
	int minesToPlant{ m_fieldMines };
	m_fieldMines = 0;	// Reset mine count since it will be re-incremented

	while (minesToPlant > 0)
	{
		randRow = rollZone(RNG::mt);
		randCol = rollZone(RNG::mt);

		if (m_zone[fieldIndex(randRow, randCol)].hasMine())
		{
			continue;
		}

		placeMine(randRow, randCol);
		--minesToPlant;
	}
}

int Field::fieldSizeInit(Difficulty diff)
{
	switch (diff)
	{
	case easy:		return	g_easyFieldSize;
	case medium:	return	g_mediumFieldSize;
	case hard:		return	g_hardFieldSize;
	default:
		std::cerr << "Invalid difficulty, setting to easy\n";
		return g_easyFieldSize;
	}
}



// Returns single index for flattened field matrix
int Field::fieldIndex(int row, int col) const
{
	return (row * m_fieldSize) + col;
}

void Field::placeMine(int row, int col)
{
	m_zone[fieldIndex(row, col)].setMine();

	++m_fieldMines;
	invokeAtAdjacents(row, col, &Field::incrementZone);
}

// Scan and invoke function at origin and adjacent zones
void Field::invokeAtAdjacents(int originRow, int originCol, memberFunc_t function)
{

	//  Z Z Z		Z = Adjacent Zone
	//  Z O Z		O = Origin Zone
	//  Z Z Z


	constexpr int maxAdjZones{ 9 };

	// Using pairs to remove need for overloaded
	// functions that use single index
	std::pair<int,int> adjZone[maxAdjZones]
	{
		std::pair{originRow - 1, originCol - 1},    // Northwest
		std::pair{originRow - 1, originCol    },    // North
		std::pair{originRow - 1, originCol + 1},    // Northeast
		std::pair{originRow	   , originCol - 1},    // West
		std::pair{originRow	   , originCol    },    // Origin
		std::pair{originRow	   , originCol + 1},    // East
		std::pair{originRow + 1, originCol - 1},    // Southwest
		std::pair{originRow + 1, originCol    },    // South
		std::pair{originRow + 1, originCol + 1},    // Southeast
	};

	for (auto scan : adjZone)
	{
		// If within array bounds
		if (((scan.first >= 0) && (scan.first < m_fieldSize))
			&& ((scan.second >= 0) && (scan.second < m_fieldSize)))
		{
			(this->*function)(scan.first, scan.second);
		}
	}
}

// Swap zone if first pick is a mine
void Field::exposeFirstZone(int row, int col)
{
	std::uniform_int_distribution rollElem{ 0, m_flatFieldSize - 1 };

	if (m_zone[fieldIndex(row, col)].hasMine())	// First zone
	{
		int randElem{};
		do
		{
			randElem = rollElem(RNG::mt);
		} while (m_zone[randElem].hasMine());	// Random zone

		std::swap(m_zone[fieldIndex(row, col)], m_zone[randElem]);
	}

	exposeZone(row, col);
}

void Field::exposeZone(int row, int col)
{
	int index{ fieldIndex(row, col) };

	// Terminates recursion for empty sections
	if (m_zone[index].isExposed())
	{
		return;
	}

	m_zone[index].expose();
	++m_exposedZones;

	// Game lose condition
	if (m_zone[index].hasMine())
	{
		m_mineExposed = true;
	}

	// Recursively expose empty sections
	if ((m_zone[index].getMineCount() == 0) && (!m_zone[index].hasMine()))
	{
		invokeAtAdjacents(row, col, &Field::exposeZone);
	}
}

// Increments adjacent mine counter at given zone
void Field::incrementZone(int row, int col)
{
	m_zone[fieldIndex(row, col)].increment();
}

// Decrements adjacent mine counter at given zone
void Field::decrementZone(int row, int col)
{
	m_zone[fieldIndex(row, col)].decrement();
}

void Field::exposeAll()
{
	for (Zone& element : m_zone)
	{
		element.expose();
	}
}

void print(const Field& field)
{
	// Convert columns from A-Z to int value
	constexpr int asciiAlphaOffset{ 65 };

	// Column headers
	std::cout << "\n\n       ";
	for (int col{}; col < field.m_fieldSize; ++col)
	{
		std::cout << static_cast<char>(col + asciiAlphaOffset) << "   ";
	}
	std::cout << "\n     ";
	for (int col{}; col < field.m_fieldSize; ++col)
	{
		std::cout << "----";
	}
	std::cout << "\n\n";

	for (int row{}; row < field.m_fieldSize; ++row)
	{
		// Row headers
		std::cout << std::setw(2) << row + 1 << "|   ";

		for (int col{}; col < field.m_fieldSize; ++col)
		{
			// Zone matrix
			std::cout << field.m_zone[field.fieldIndex(row, col)] << ' ';
		}

		std::cout << "\n\n";
	}
}

int Field::squareSize() const { return m_fieldSize; }

bool Field::mineExposed() const { return m_mineExposed; }

void Field::flagZone(int row, int col)
{
	m_zone[fieldIndex(row, col)].toggleFlag();
}

// Returns true if all non-mine zones are exposed
bool Field::isCleared() const
{
	// Should only be false-positive if mine has been exposed
	return m_exposedZones == ((m_fieldSize * m_fieldSize) - m_fieldMines);
}

// Exposes zone and removes mine
void Field::blastZone(int row, int col)
{
	const int zoneIdx{ fieldIndex(row, col) };

	// Blast removes mine
	if (m_zone[zoneIdx].hasMine())
	{
		invokeAtAdjacents(row, col, &Field::decrementZone);
		--m_fieldMines;
	}
	
	// Prevents win from blasting exposed zones repeatedly
	if (!m_zone[zoneIdx].isExposed())
	{
		++m_exposedZones;
	}

	m_zone[zoneIdx].blast();

	// Scorched zones should not give any assistance
	if (!m_zone[zoneIdx].isScorched() && (m_zone[zoneIdx].getMineCount() == 0))
	{
		invokeAtAdjacents(row, col, &Field::exposeZone);
	}
}

void Field::invokeHorizontal(int originRow, int originCol, memberFunc_t function)
{
	constexpr int maxAdjZones{ 5 };

	// Using pairs to remove need for overloaded
	// functions that use single index
	std::pair<int, int> adjZone[maxAdjZones]
	{
		std::pair{originRow, originCol - 2},    // West - 1
		std::pair{originRow, originCol - 1},    // West
		std::pair{originRow, originCol    },    // Origin
		std::pair{originRow, originCol + 1},    // East
		std::pair{originRow, originCol + 2},    // East + 1
	};

	for (auto scan : adjZone)
	{
		// If within array bounds
		if (((scan.first >= 0) && (scan.first < m_fieldSize))
			&& ((scan.second >= 0) && (scan.second < m_fieldSize)))
		{
			(this->*function)(scan.first, scan.second);
		}
	}
}

void Field::invokeVertical(int originRow, int originCol, memberFunc_t function)
{
	constexpr int maxAdjZones{ 5 };

	// Using pairs to remove need for overloaded
	// functions that use single index
	std::pair<int, int> adjZone[maxAdjZones]
	{
		std::pair{originRow - 2, originCol},    // North - 1
		std::pair{originRow - 1, originCol},    // North
		std::pair{originRow	   , originCol},    // Origin
		std::pair{originRow + 1, originCol},    // South
		std::pair{originRow + 2, originCol},    // South + 1
	};

	for (auto scan : adjZone)
	{
		// If within array bounds
		if (((scan.first >= 0) && (scan.first < m_fieldSize))
			&& ((scan.second >= 0) && (scan.second < m_fieldSize)))
		{
			(this->*function)(scan.first, scan.second);
		}
	}
}

// Blast horizontal line of 5 zones, centered on selected zone
void Field::horizontalBlast(int row, int col)
{
	invokeHorizontal(row, col, &Field::blastZone);
}

// Blast vertical line of 5 zones, centered on selected zone
void Field::verticalBlast(int row, int col)
{
	invokeVertical(row, col, &Field::blastZone);
}

// Blast 3x3 square of zones, centered on selected zone
void Field::radialBlast(int row, int col)
{
	invokeAtAdjacents(row, col, &Field::blastZone);
}

// Reconstruct grid and redistribute mines and exposes
// Mines exposed int the process will instead be flagged
void Field::jamboree([[maybe_unused]] int row, [[maybe_unused]] int col)
{
	*this = Field{ m_difficulty, m_exposedZones, m_fieldMines };

	std::uniform_int_distribution rollZone{ 0, m_fieldSize - 1 };

	int zonesToTarget{ m_exposedZones };	// New value = random exposes + flags placed by jamboree

	m_exposedZones = 0;		// Reset to recalculate since some will convert to flags

	int randRow{};
	int randCol{};
	int currentRandIdx{};
	while (zonesToTarget > 0)
	{
		randRow = rollZone(RNG::mt);
		randCol = rollZone(RNG::mt);
		currentRandIdx = fieldIndex(randRow, randCol);
		if (m_zone[currentRandIdx].hasMine())
		{
			flagZone(randRow, randCol);
		}
		else
		{
			blastZone(randRow, randCol);
		}

		--zonesToTarget;
	}
}

void Field::usePower(int row, int col)
{
	constexpr int maxPowers{ 4 };

	// All powers require row and column parameters,
	// even if unused, to share identical type
	using powerArr_t = void (Field::* const[maxPowers])(int, int);
	
	static powerArr_t powers
	{
		&Field::horizontalBlast,
		&Field::verticalBlast,
		&Field::radialBlast,
		&Field::jamboree
	};

	std::uniform_int_distribution randPower{ 0, maxPowers - 1 };

	(this->*powers[randPower(RNG::mt)])(row, col);
}