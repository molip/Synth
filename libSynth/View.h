#pragma once

namespace Synth
{
	namespace UI
	{
	class View
	{
	public:
		View() {}
		virtual ~View() {}

		virtual void InvalidateAll() = 0;
		virtual void SetCapture(bool capture) = 0;
		virtual void CancelValueEdit() = 0;
	};
	}
}