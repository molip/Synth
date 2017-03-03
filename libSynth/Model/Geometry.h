#pragma once

#include <ostream>
#include <istream>

namespace Synth
{
namespace Model
{
	class Point
	{
	public:
		Point() : x(0), y(0) {}
		Point(const Point& rhs) = default;
		Point(int _x, int _y) : x(_x), y(_y) {}

		bool operator ==(const Point& rhs) const
		{
			return x == rhs.x && y == rhs.y;
		}

		Point operator +(const Point& rhs) const
		{
			return Point(x + rhs.x, y + rhs.y);
		}

		int x, y;
	};

	std::ostream& operator<<(std::ostream& stream, const Point& val);
	std::istream& operator>>(std::istream& stream, Point& val);

	class Rect
	{
		friend std::ostream& operator<<(std::ostream& stream, const Rect& val);
		friend std::istream& operator>>(std::istream& stream, Rect& val);
	public:
		Rect();
		Rect(const Point& p);
		Rect(const Point& p0, const Point& p1);
		Rect(int left, int top, int right, int bottom);
		~Rect();

		int Width() const { return _p1.x - _p0.x; }
		int Height() const { return _p1.y - _p0.y; }
		int Left() const { return _p0.x; }
		int Right() const { return _p1.x; }
		int Top() const { return _p0.y; }
		int Bottom() const { return _p1.y; }
		int& Left() { return _p0.x; }
		int& Right() { return _p1.x; }
		int& Top() { return _p0.y; }
		int& Bottom() { return _p1.y; }
		Point GetCentre() const { return Point(_p0.x + (_p1.x - _p0.x) / 2, _p0.y + (_p1.y - _p0.y) / 2); }
		bool Contains(const Point& point) const;
		bool IsEmpty() const { return _p0 == _p1; }

		void Normalise();
		void Inflate(int x, int y);
	
	private:
		Point _p0, _p1;
	};

	std::ostream& operator<<(std::ostream& stream, const Rect& val);
	std::istream& operator>>(std::istream& stream, Rect& val);
}
}
