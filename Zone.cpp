#include "Zone.h"
#include "RNG.h"		// randPercent 1-100 int generator

Zone::Zone(bool mine)
	:m_hasMine{mine}
{}

void Zone::setMine() { m_hasMine = true; }

void Zone::removeMine() { m_hasMine = false; }

void Zone::increment() { ++m_closeMines; }

void Zone::decrement() { --m_closeMines; }

void Zone::toggleFlag() { m_hasFlag = !m_hasFlag; }

bool Zone::hasMine() const { return m_hasMine; }

int Zone::getMineCount() const { return m_closeMines; }

void Zone::expose() { m_isExposed = true; }

void Zone::blast()
{
	constexpr int scorchPercentChance{ 25 };

	if(RNG::randPercent(RNG::mt) <= scorchPercentChance)
		m_isScorched = true;
	
	expose();
	removeMine();
}

bool Zone::isExposed() const { return m_isExposed; }

bool Zone::isScorched() const { return m_isScorched; }

std::ostream& operator<< (std::ostream& out, const Zone& zone)
{
	if (zone.m_isExposed)
	{
		if (zone.m_hasMine)
			out << "{X}";

		else if (zone.m_isScorched)
			out << " # ";

		// Never more than 8 adjacent mines
		else if (zone.m_closeMines > 0)
			out << ' ' << zone.m_closeMines << ' ';

		else
			out << "   ";
	}
	else // Not exposed
	{
		if (zone.m_hasFlag)
		out << "[&]";
		else
		out << "[ ]";
	}

	return out;
}