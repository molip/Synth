#pragma once

#include <string>

using Tag = std::string;

//class Tag
//{
//public:
//	Tag(int id = 0) : _id(id) {}
//
//	//bool operator == (long id) const { return id == _id; }
//	//bool operator != (long id) const { return id != _id; }
//
//	bool operator == (const Tag& rhs) const { return _id == rhs._id; }
//	bool operator != (const Tag& rhs) const { return _id != rhs._id; }
//
//	std::string ToString() const { return std::string(reinterpret_cast<const char*>(&_id), 4); }
//
//private:
//	int _id;
//};