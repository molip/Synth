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
}
}
