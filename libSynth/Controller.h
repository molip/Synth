#pragma once

#include "Model/Geometry.h"
#include "Model/Graph.h"
#include "Types.h"
#include "MIDIKeyboard.h"
#include "ModuleIkon.h"
#include "Settings.h"

#include "../libKernel/IndexRange.h"

#include <memory>
#include <vector>

namespace Synth
{
	namespace Model
	{
		class Graph;
		class Module;
		class ValueType;
	}
	class CommandStack;
	class Player;

	namespace UI
	{
	class View;
	class Controller : private Model::GraphObserver
	{
	public:
		Controller(const MIDIKeyboard::KeyCodes& keyCodes);
		~Controller();

		void SetView(View* view) { _view = view; }
		View* GetView() { return _view; }

		void SetPlayer(Player* player) { _player = player; }
		Player* GetPlayer() { return _player; }

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

		void OnKeyDown(char key);
		void OnKeyUp(char key);

		void CommitValueEdit(const std::string& text);

		using ModuleIkonRange = Kernel::IndexRange<ModuleIkon>;
		ModuleIkonRange GetModuleIkons() const;

		using Connection = std::pair<Model::Point, Model::Point>;
		const Connection* GetLiveConnection() const { return _liveConnection.get(); }
		std::vector<Connection> GetConnections() const;

		bool Export() const;
		bool ExportPolyTest();
		bool ExportMIDIFile(const std::wstring& path) const;
		void StopMIDIFilePlayback() const;
		void ExportRawMIDI(Buffer&& buffer);
			
		bool Save(const std::wstring& path) const;
		bool Load(const std::wstring& path);

		class Selection
		{
		public:
			int moduleID = 0;
			Model::Tag pinID;
			enum class Element { None, Input, Output, Offset, Scale } element {};
		};

		// GraphObserver 
		virtual void OnGraphNotification(const Model::Notification& notification) override;

		static const Settings& GetSettings() { return _settings; }
		void SetSettings(const Settings& settings);

	private:
		Selection HitTest(Model::Point point, Model::Rect* elementRect = nullptr) const;
		bool SendData(const Buffer& buffer) const;
		bool DoExport(byte polyphony) const;
		int& GetInputParamsValue(Model::InputParams& params, Selection::Element element) const;
		void SetInputParams(Selection& sel, std::function<int(const Model::ValueType&, int)> fn);
		void ExportSettings() const;

		View* _view = nullptr;
		Player* _player = nullptr;

		std::unique_ptr<Model::Graph> _graph;
		std::unique_ptr<CommandStack> _commandStack;

		std::unique_ptr<Model::Point> _mouseDownPoint;
		Selection _selection;

		std::unique_ptr<Connection> _liveConnection;

		enum class SyncState { None, Local, All };
		mutable SyncState _syncState = SyncState::None;

		MIDIKeyboard _midiKeyboard;

		static Settings _settings;
	};
	}
}
