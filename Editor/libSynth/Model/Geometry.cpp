#include "stdafx.h"
#include "Geometry.h"

using namespace Model;

std::ostream& Model::operator<<(std::ostream& stream, const Point& val)
{
	stream << "(" << val.x << "," << val.y << ")";

	return stream;
}

std::istream& Model::operator>>(std::istream& stream, Point& val)
{
	char lparen, comma, rparen;
	int x, y;
	stream >> lparen >> x >> comma >> y >> rparen;

	if (lparen == '(' && comma == ',' && rparen == ')')
		val.x = x, val.y = y;

	return stream;
}



Rect::Rect()
{
}

Rect::Rect(const Point& p) : _p0(p), _p1(p)
{
}

Rect::Rect(const Point& p0, const Point& p1) : _p0(p0), _p1(p1)
{
}

Rect::~Rect()
{
}

bool Rect::Contains(const Point& p) const
{
	return p.x >= _p0.x && p.x <= _p1.x && p.y >= _p0.y && p.y <= _p1.y;
}

void Rect::Normalise()
{
	if (_p0.x > _p1.x)
		std::swap(_p0.x, _p1.x);

	if (_p0.y > _p1.y)
		std::swap(_p0.y, _p1.y);
}


void Rect::Inflate(int x, int y)
{
	_p0.x -= x;
	_p0.y -= y;
	_p1.x += x * 2;
	_p1.y += y * 2;
}

std::ostream& Model::operator<<(std::ostream& stream, const Rect& val)
{
	stream << val._p0 << " - " << val._p1;

	return stream;
}

std::istream& Model::operator>>(std::istream& stream, Rect& val)
{
	char hyphen;
	Point p0, p1;
	stream >> p0 >> hyphen >> p1;

	if (hyphen == '-')
		val._p0 = p0, val._p1 = p1;

	return stream;
}

