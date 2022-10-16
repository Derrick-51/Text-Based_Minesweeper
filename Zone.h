#ifndef ZONE_H
#define ZONE_H

#include <iostream>

class Zone
{
public:
	Zone(bool mine = false);

	void setMine();

	void removeMine();

	void increment();

	void decrement();

	void toggleFlag();

	bool hasMine() const;

	int getMineCount() const;

	void expose();

	void blast();

	bool isExposed() const;

	bool isScorched() const;

	friend std::ostream& operator<< (std::ostream& out, const Zone& zone);

private:

	bool m_hasMine{};
	bool m_hasFlag{};
	bool m_isExposed{};
	int m_closeMines{ 0 };
	bool m_isScorched{};
};

#endif