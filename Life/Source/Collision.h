/////////////////////////////////////////////////////////
// Collision.h
//
// Track collisions between environment objects
//
// Use Cases
//
// Walk all biots looking for collisions
//		Record collisions, each seperately
//		
//
//
#pragma once

class CollidingLines
{
public:
	CollidingLines()
	{
		m_nHits = 0;
	}

	// The line involved in the collision
	// from this biot
	int m_thisLine;

	// The line involved in the collision from the other biot
	int m_thatLine;

	// The type of collision 
	int m_collisionType;

	// Consecutive Hits
	int m_hits;
};


class CollidingPair
{
public:
	DWORD m_thisId;
	Biot* m_thisBiot;
	DWORD m_thatId;
	Biot* m_thatBiot;
};


class CollidingArray
{



};

