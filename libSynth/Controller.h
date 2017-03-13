#pragma once

#include "Model/Geometry.h"
#include "Types.h"
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

		void InsertModule(const std::string& type);
		void DeleteModule();
		bool CanDeleteModule() const;

		bool OnMouseMove(Model::Point point);
		bool OnLButtonDown(Model::Point point);
		void OnLButtonUp(Model::Point point);

		using ModuleIkonRange = Kernel::IndexRange<ModuleIkon>;
		ModuleIkonRange GetModuleIkons() const;

		using Connection = std::pair<Model::Point, Model::Point>;
		const Connection* GetLiveConnection() const { return _liveConnection.get(); }
		std::vector<Connection> GetConnections() const;

		BufferPtr Export() const;
		BufferPtr ExportMIDIFile(const std::wstring& path) const;
		bool Save(const std::wstring& path) const;
		bool Load(const std::wstring& path);

		class Selection
		{
		public:
			int moduleID = 0;
			Model::Tag pinID;
			bool isOutput = false;
		};

	private:
		Selection HitTest(Model::Point point) const;

		std::unique_ptr<Model::Graph> _graph;
		std::unique_ptr<CommandStack> _commandStack;

		std::unique_ptr<Model::Point> _mouseDownPoint;
		Selection _selection;

		std::unique_ptr<Connection> _liveConnection;
	};
	}
}