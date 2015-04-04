import QtQuick 2.0
import com.github.galymzhan 0.1
import 'Flux.js' as Flux

QtObject {
	property string dispatchToken

	// Active note (or null if none)
	property Note note: null

	// Model that feeds data to a listview
	property ListModel model: ListModel {}

	property var _searchResults: ({})

	Component.onCompleted: {
		dispatchToken = Flux.dispatcher.register(function(action) {
			var index
			switch (action.type) {
				case Flux.Actions.SEARCH_NOTE:
					if (note) {
						repository.persistNote(note)
					}
					model.clear()
					_searchResults = {}
					var list = repository.search(action.query)
					while (list.hasMore()) {
						var row = list.fetch()
						model.append({ id: row.getId(), title: row.getTitle(), updatedAt: row.getUpdatedAt() })
						_searchResults[row.getId()] = row
					}
					break

				case Flux.Actions.SELECT_NOTE:
					if (note) {
						repository.persistNote(note)
					}
					note = _searchResults[action.id]
					break

				case Flux.Actions.CREATE_NOTE:
					if (note) {
						repository.persistNote(note)
					}
					var created = repository.createNote()
					_searchResults[created.getId()] = created
					model.insert(0, { id: created.getId(), title: created.getTitle(), updatedAt: created.getUpdatedAt() })
					note = created
					break

				case Flux.Actions.UPDATE_NOTE:
					if (note) {
						var changed = note.setContent(action.content)
						if (!changed) return false
						note.setUpdatedAt(Date.now() / 1000)
						index = _findModelIndex(note.getId())
						if (index >= 0) {
							model.set(index, { title: note.getTitle(), updatedAt: note.getUpdatedAt() })
						}
						if (index > 0) model.move(index, 0, 1)
					}
					break

				case Flux.Actions.PERSIST_NOTE:
					if (note) {
						repository.persistNote(note)
					}
					break

				case Flux.Actions.DELETE_NOTE:
					if (note) {
						var id = note.getId()
						index = _findModelIndex(id)
						if (index >= 0) model.remove(index)
						repository.deleteNote(note)
						if (_searchResults[id]) delete _searchResults[id]
						note = null
					}
					break

				case Flux.Actions.CONNECT_DATABASE:
					_searchResults = {}
					note = null
					model.clear()
			}
		})
	}

	function _findModelIndex(id) {
		for (var i = 0; i < model.count; i++) {
			if (model.get(i).id === id) {
				return i
			}
		}
		return -1
	}
}
