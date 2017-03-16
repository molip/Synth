#pragma once

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
		virtual void StartValueEdit(const Synth::Model::Rect& rect, const std::string& str) = 0;
	};
	}
}