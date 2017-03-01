#pragma once

#include "Model/Geometry.h"

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

	class Controller
	{
	public:
		Controller();
		~Controller();
		bool CanUndo() const;
		bool CanRedo() const;
		void Undo();
		void Redo();

		void OnMouseMove(Model::Point point);
		void OnLButtonDown(Model::Point point);
		void OnLButtonUp(Model::Point point);

		const std::vector<Model::Module*>& GetSorted() const; // TODO: Change this.
		Model::Rect GetModuleRect(const Model::Module& mod) const;

	private:
		Model::Module* HitTest(Model::Point point) const;

		std::unique_ptr<Model::Graph> _graph;
		std::unique_ptr<CommandStack> _commandStack;

	    std::unique_ptr<Model::Point> _mouseDownPoint;
		Model::Module* _selectedModule;
	};
}