#pragma once

#include "Model/Geometry.h"
#include "Model/Graph.h"
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
	class View;
	class Controller : private Model::GraphObserver
	{
	public:
		Controller();
		~Controller();

		void SetView(View* view) { _view = view; }
		View* GetView() { return _view; }

		bool CanUndo() const;
		bool CanRedo() const;
		void Undo();
		void Redo();

		void InsertModule(const std::string& type);
		void DeleteModule();
		bool CanDeleteModule() const;

		void OnMouseMove(Model::Point point);
		void OnLButtonDown(Model::Point point);
		void OnLButtonUp(Model::Point point);
		void OnMouseWheel(Model::Point point, bool negative, bool coarse);

		void CommitValueEdit(const std::string& text);

		using ModuleIkonRange = Kernel::IndexRange<ModuleIkon>;
		ModuleIkonRange GetModuleIkons() const;

		using Connection = std::pair<Model::Point, Model::Point>;
		const Connection* GetLiveConnection() const { return _liveConnection.get(); }
		std::vector<Connection> GetConnections() const;

		bool Export() const;
		bool TestGraph() const;
		bool ExportPolyTest();
		bool ExportMIDIFile(const std::wstring& path) const;
		void StopMIDIFilePlayback() const;
		bool Save(const std::wstring& path) const;
		bool Load(const std::wstring& path);

		class Selection
		{
		public:
			int moduleID = 0;
			Model::Tag pinID;
			enum class Element { None, Input, Output, Value } element {};
		};

		// GraphObserver 
		virtual void OnGraphNotification(const Model::Notification& notification) override;

	private:
		Selection HitTest(Model::Point point, Model::Rect* elementRect = nullptr) const;
		bool DoExport(byte polyphony) const;

		View* _view = nullptr;

		std::unique_ptr<Model::Graph> _graph;
		std::unique_ptr<CommandStack> _commandStack;

		std::unique_ptr<Model::Point> _mouseDownPoint;
		Selection _selection;

		std::unique_ptr<Connection> _liveConnection;

		mutable bool _inSync = false;
	};
	}
}
