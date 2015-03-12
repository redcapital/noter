import QtQuick 2.0
import com.github.galymzhan 0.1

QtObject {
	property var dispatcher

	// Active note (or null if none)
	property var note: null

	// Model that feeds data to a listview
	readonly property var model: noteListModel

	signal indexChanged(int newIndex)

	function init(_dispatcher) {
		dispatcher = _dispatcher
		var token = dispatcher.register(function(action) {
			switch (action.type) {
				case Actions.SEARCH_NOTE:
					if (note) {
						repository.persistNote(note);
					}
					noteListModel.search(action.query);
					break;

				case Actions.SELECT_NOTE:
					if (note) {
						repository.persistNote(note);
					}
					note = noteListModel.get(action.index)
					indexChanged(action.index)
					break;

				case Actions.CREATE_NOTE:
					if (note) {
						repository.persistNote(note);
					}
					noteListModel.create();
					note = noteListModel.get(0)
					indexChanged(0)
					break;

				case Actions.UPDATE_NOTE:
					if (note) {
						noteListModel.update(note, action.content);
					}
					break;

				case Actions.PERSIST_NOTE:
					if (note) {
						repository.persistNote(note);
					}
					break;

				case Actions.DELETE_NOTE:
					if (note) {
						noteListModel.deleteNote(note)
						note = null
						indexChanged(-1)
					}
					break;
			}
		});
	}
}
