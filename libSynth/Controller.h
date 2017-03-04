#pragma once

#include "Model/Geometry.h"
#include "ModuleIkon.h"

#include "../libKernel/IndexRange.h"

#include <memory>
#include <vector>

namespace Synth
{
	namespace Model
	{
		class Graph;
		class Module;
	}
	class CommandStack;

	namespace UI
	{
	class Controller
	{
	public:
		Controller();
		~Controller();
		bool CanUndo() const;
		bool CanRedo() const;
		void Undo();
		void Redo();

		bool OnMouseMove(Model::Point point);
		void OnLButtonDown(Model::Point point);
		void OnLButtonUp(Model::Point point);

		using ModuleIkonRange = Kernel::IndexRange<ModuleIkon>;
		ModuleIkonRange GetModuleIkons() const;

	private:
		int HitTest(Model::Point point) const;

		std::unique_ptr<Model::Graph> _graph;
		std::unique_ptr<CommandStack> _commandStack;

		std::unique_ptr<Model::Point> _mouseDownPoint;
		int _selectedModuleID = 0;
	};
	}
}