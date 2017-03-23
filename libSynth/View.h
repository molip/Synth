#pragma once

#include "Types.h"

#include <string>

namespace Synth
{
	namespace Model
	{
		class Rect;
	}

	namespace UI
	{
	class View
	{
	public:
		View() {}
		virtual ~View() {}

		virtual void InvalidateAll() = 0;
		virtual void SetCapture(bool capture) = 0;
		virtual void StartValueEdit(const Model::Rect& rect, const std::string& str) = 0;
		virtual bool UploadData(const Buffer& buffer) = 0;
		virtual void SetModified(bool modified) = 0;
	};
	}
}
