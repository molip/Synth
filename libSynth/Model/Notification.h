#pragma once

#include "Module.h"
#include "Tag.h"
#include "Defs.h"

namespace Synth
{
namespace Model
{
	class Notification
	{
	public:
		virtual ~Notification() = default;
	};

	class StructureChangedNotification : public Notification {};

	class ValueChangedNotification : public Notification 
	{
	public:
		ValueChangedNotification(int modID_, Tag pinID_) : modID(modID_), pinID(pinID_) {}
		
		int modID;
		Tag pinID;
	};

}
}
