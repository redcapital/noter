import QtQuick 2.0
import com.github.galymzhan 0.1
import 'Flux.js' as Flux

QtObject {
	property var dispatcher

	// Active note (or null if none)
	property Note note: null

	// Model that feeds data to a listview
	readonly property var model: noteListModel

	signal indexChanged(int newIndex)

	Component.onCompleted: {
		Flux.dispatcher.register(function(action) {
			switch (action.type) {
				case Flux.Actions.SEARCH_NOTE:
					if (note) {
						repository.persistNote(note)
					}
					noteListModel.search(action.query)
					break

				case Flux.Actions.SELECT_NOTE:
					if (note) {
						repository.persistNote(note)
					}
					note = noteListModel.get(action.index)
					indexChanged(action.index)
					break

				case Flux.Actions.CREATE_NOTE:
					if (note) {
						repository.persistNote(note)
					}
					noteListModel.create()
					note = noteListModel.get(0)
					indexChanged(0)
					break

				case Flux.Actions.UPDATE_NOTE:
					if (note) {
						noteListModel.update(note, action.content)
					}
					break

				case Flux.Actions.PERSIST_NOTE:
					if (note) {
						repository.persistNote(note)
					}
					break

				case Flux.Actions.DELETE_NOTE:
					if (note) {
						noteListModel.deleteNote(note)
						note = null
						indexChanged(-1)
					}
					break
			}
		})
	}
}
