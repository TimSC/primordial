//////////////////////////////////////////////////////////////////////
//
// vector.h
//
// Math functions associated with motion and acceleration
//

#pragma once
#include "Etools.h"

////////////////////////////////////////////////////////////////////////////////////
// Class Vector
//
//
//
const double RADIANS = PI / 180.0;
const double limit   = 3.0F;  // 3
const double rlimit  = 3.0F;  // 12 diameter of 100 rotating 8 deg

class VectorSum;

class Vector
{
protected:
    double dx;	// Rate of change of position X
    double dy;  // Rate of change of position Y
    double x;	// Our position X
    double y;   // Our position Y
    double dr;  // Rotation rate in degrees
    double r;   // Rotation position in degrees

	// These variables are calculated based on the difference
	// of the origin from the center of mass
	double drx; // Change position X based on rotation rate
	double dry; // Change position Y based on rotation rate

public:
    double mass;

public:

    Vector(void);

	virtual void Serialize(CArchive& ar);

    // Proposes movement in rotation, and translation
	// Always call tryRotate first!!
    int tryRotate(const BPoint& origin, const BPoint& center);
    int tryStepX();
    int tryStepY();

	// Actually make movement
    void makeStep(void);

    int GetX() { return (int) x; }
    int GetY() { return (int) y; }
    void backStepX(void) { x -= dx; }
    void backStepY(void) { y -= dy; }
    void backRotate(void){ r -= dr; }

    void invertDeltaY(void) { dy = -dy; }
    void invertDeltaX(void) { dx = -dx; }

	double CheckLimit(const double value) const
	{
		if (value > limit)
			return limit;
		else
			if (value < -limit)
				return -limit;

		return value;
	}
	
	double CheckRLimit(const double value) const
	{
		if (value > rlimit)
			return rlimit;
		else
			if (value < -rlimit)
				return -rlimit;

		return value;
	}
	
	void setDeltaX(const double DX)
	{
		dx = CheckLimit(DX);
	}
    void setDeltaX(const int DX)
	{
		dx = CheckLimit((double) DX);
	}
    void setDeltaY(const double DY)
	{
		dy = CheckLimit(DY);
	}
    void setDeltaY(const int DY)
	{
		dy = CheckLimit((double) DY);
	}
    void setDeltaRotate(const double DR)
	{ 
		dr = CheckRLimit(DR);
	}
    void setDeltaRotate(const int DR)
	{ 
		dr = CheckRLimit((double) DR);
	}
	void setMass(double MASS) { mass = MASS; }
	void addMass(double MASS) { mass += MASS; }

    void adjustDeltaX(double DX){dx += DX;}
    void adjustDeltaY(double DY)      { dy += DY; }
    void adjustDeltaRotate(double DR) { dr += DR; }

    void accelerateX(double ddx)
	{
		dx = CheckLimit(dx + ddx / mass);
		/*(((double)ddx) / mass);*/
	}

    void accelerateY(double ddy)
	{
		/*(((double)ddy) / mass);*/ 
		dy = CheckLimit(dy + ddy / mass);
	}
    void accelerateRotation(double ddr)
	{ 
		dr = CheckRLimit(dr + ddr / mass);
	}

    void friction(double fric);

    double getDeltaX(void) const { return dx; }
    double getDeltaY(void) const { return dy; }
    double getDeltaRotate(void) { return dr;  }
    int getRotate(void)      { return (int) r; }

    void setX(int X)      { x = (double) X; }
    void setY(int Y)      { y = (double) Y; }
    void setRotate(int R) { r = (double) R; }

    double collisionX(Vector &enemy)
    {
      return ((mass - enemy.mass) * dx + 2 * enemy.mass * enemy.dx) / (mass + enemy.mass);
    }

	// Mass Impact equation from page 290 of J.P. Den Hartog "Mechanics"
	// Returns the resultant velocity vector
	double collisionResult(const double emass, const double DX, const double eDX) const
	{
		return ((mass - emass) * DX + 2 * emass * eDX) / (mass + emass);
	}

    double collisionY(Vector &enemy)
    {
      return ((mass - enemy.mass) * dy + 2 * enemy.mass * enemy.dy) / (mass + enemy.mass);
    }

    double distance(const int x1, const int y1) const { return sqrt((double)((x1*x1) + (y1*y1))); }
    double distance(const double x1, const double y1) const { return sqrt((x1*x1) + (y1*y1)); }

    double rotationComponent(const int x1, const int y1, const int x2, const int y2) const
    { 
      double dist = distance(x1, y1);
      if (dist)
        return ((double)((y1 * x2)-(x1 * y2))) / dist;
      else
        return 0;
    }

    double rotationComponent(double radius, double x1, double y1, double x2, double y2)
    {
		if (radius == 0.0)
			return 0.0;

        return (((x1 * y2) - (y1 * x2))) / radius;
    }

    double motionComponent(double vector, double rotation)
	{
		return (fabs(fabs(vector) - fabs(rotation)) < .0001)?0.0:sqrt((vector * vector) - (rotation * rotation));
	}

    double fraction(double motion, double x1, double center)
	{
		return (motion * x1) / center;
	}

	// Circumference per degree times the number of degrees per turn provides a vector.
    double VectorR(const double radius) const { return (RADIANS * radius * dr); }

	// Cos (alpha) = deltaX / radius  &  Cos(alpha) = Yr / Vr, solved for Yr
    double deltaYr(const double Vr, const double& deltaX, const double radius) const
	{
		return (radius != 0)?Vr * deltaX / radius: 0;
	}

    double deltaXr(const double Vr, const double& deltaY, const double radius) const
	{
		return (radius != 0)?-Vr * deltaY / radius: 0;
	}

	// a positive dr causes the biot to move clockwise on the screen
	// a delta y below the origin (greater y value) is a negative deltaY
	void RotatedDelta(double& Vx, double& Vy, const double& deltaX, const double& deltaY, const double radius) const
	{
		// Current velocity vector at this radius
		double Vr = VectorR(radius);

		// Our cumulative velocity in the DX and DY directions
		// to impart to the other object.
		Vx = getDeltaX() + deltaXr(Vr, deltaY, radius);
		Vy = getDeltaY() + deltaYr(Vr, deltaX, radius);
	}                    

	void TraceDebug();

	void ValidateBounds(bool bTop, bool bBottom, bool bLeft, bool bRight);
	void MakeNegative(double& d);
	void MakePositive(double& d);
};


inline void Vector::friction(double fric)
{
	dx -= (fric * dx);
	dy -= (fric * dy);
	dr -= (fric * dr);
}

// Propose next rotation movement, but don't do it yet
inline int Vector::tryRotate(const BPoint& origin, const BPoint& center)
{
	CLine line(center, origin);

	double deltaC = distance(origin.x - center.x, origin.y - center.y);
	double deltaR = RADIANS * dr + line.Angle();

	drx = (center.x + (deltaC * cos(deltaR))) - origin.x;
	dry = (center.y + (deltaC * sin(deltaR))) - origin.y;

	return (int(r + dr) - int(r));
}


// Propose next X movement, but don't do it yet
inline int Vector::tryStepX(void)
{
	return int(x + dx + drx) - int(x);
}


// Propose next Y movement, but don't do it yet
inline int Vector::tryStepY(void)
{
	return int(y + dy + dry) - int(y);
}


// Actually make the step
inline void Vector::makeStep(void)
{
	x += (dx + double(drx));
	y += (dy + double(dry));
	r += dr;

	if (r >= 360.0)
	{
		r -= 360.0;
	}
	else
	{
		if (r <= -360.0)
			r += 360.0;
	}
}


inline void Vector::ValidateBounds(bool bTop, bool bBottom, bool bLeft, bool bRight)
{
	// If we are over the top, and heading north, go south
	if (bTop)
	{
//		TRACE("Top ");
		MakePositive(dy);
	}

	if (bBottom)
	{
//		TRACE("Bottom ");
		MakeNegative(dy);
	}

	if (bLeft)
	{
//		TRACE("Left ");
		MakePositive(dx);
	}

	if (bRight)
	{
//		TRACE("Right ");
		MakeNegative(dx);
	}
}


// Encourage migration back into the field
inline void Vector::MakeNegative(double& d)
{
	if (d > 0.0)
	{
//		TRACE("Make Negative");
		d = -d;
	}

	if (d > -0.1)
	{
//		TRACE("Make Negative -.1");
		d = -0.1;
	}
//	TRACE("\n");
}


// Encourage migration back into the field
inline void Vector::MakePositive(double& d)
{
    if (d < 0.0)
	{
//		TRACE("Make Positive");
		d = -d;
	}

	if (d < 0.1)
	{
//		TRACE("Make Positive .1");
		d = 0.1;
	}
//	TRACE("\n");
}


///////////////////////////////////////////////////
// VectorSum
//
// Keeps track of multiple hits on one object
//
//
class VectorSum 
{
public:
	VectorSum() : m_hits(0) {};

	void Add(double dx, double dy, double dr);
	void Adjust(Vector& vector);
	void Set(Vector& vector);
	void Clear();

protected:
	double m_dx;
	double m_dy;
	double m_dr;
	int    m_hits;
};


// It is possible that two borders get set

inline void VectorSum::Set(Vector& vector)
{
	if (m_hits > 0)
	{
		vector.setDeltaX(m_dx);
		vector.setDeltaY(m_dy);
		vector.setDeltaRotate(m_dr);
//		TRACE("VectorSum::Set(%.3f, %.3f, %.3f)\n", vector.getDeltaX(), vector.getDeltaY(), vector.getDeltaRotate());
	}
}

inline void VectorSum::Adjust(Vector& vector)
{
	if (m_hits > 0)
	{
		vector.setDeltaX(vector.getDeltaX() + m_dx);
		vector.setDeltaY(vector.getDeltaY() + m_dy);
		vector.setDeltaRotate(vector.getDeltaRotate() + m_dr);
//		TRACE("VectorSum::Adjust(%.3f, %.3f, %.3f)\n", m_dx, m_dy, m_dr);
	}
}


inline void VectorSum::Add(double dx, double dy, double dr)
{
	if (m_hits++ == 0)
	{
		m_dx = dx;
		m_dy = dy;
		m_dr = dr;
	}
	else
	{
		m_dx = ((m_dx * (m_hits - 1)) + dx) / m_hits;
		m_dy = ((m_dy * (m_hits - 1)) + dy) / m_hits;
		m_dr = ((m_dr * (m_hits - 1)) + dr) / m_hits;
	}
}

inline void VectorSum::Clear()
{
	m_hits = 0;
}
