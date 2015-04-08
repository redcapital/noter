import QtQuick 2.0
import 'Flux.js' as Flux

QtObject {
	property string dispatchToken

	// Callbacks
	readonly property var createTag: function(name) {
		return repository.createTag(name)
	}

	readonly property var autocompleteTag: function(name, ids) {
		var resultSet = repository.autocompleteTag(name, ids), results = []
		if (resultSet) {
			for (var i = 0, c = resultSet.size(); i < c; i++) {
				results.push(resultSet.get(i))
			}
		}
		return results
	}

	signal tagListReloaded(var tagList)

	Component.onCompleted: {
		dispatchToken = Flux.dispatcher.register(function(action) {
			switch (action.type) {
				case Flux.Actions.SELECT_NOTE:
					Flux.dispatcher.waitFor([noteStore.dispatchToken])
					var resultSet = noteStore.note.getTags(), newList = []
					for (var i = 0, c = resultSet.size(); i < c; i++) {
						newList.push(resultSet.get(i))
					}
					tagListReloaded(newList)
					break

				case Flux.Actions.CREATE_NOTE:
					Flux.dispatcher.waitFor([noteStore.dispatchToken])
					tagListReloaded([])
					break

				case Flux.Actions.DELETE_NOTE:
					Flux.dispatcher.waitFor([noteStore.dispatchToken])
					tagListReloaded([])
					break

				case Flux.Actions.CONNECT_DATABASE:
					tagListReloaded([])
					break

				case Flux.Actions.TAG:
					if (!noteStore.note) {
						console.warn("Tag action shouldn't happen when there isn't a loaded note")
						return
					}
					repository.addTag(noteStore.note, action.tagId)
					break

				case Flux.Actions.UNTAG:
					if (!noteStore.note) {
						console.warn("Untag action shouldn't happen when there isn't a loaded note")
						return
					}
					repository.removeTag(noteStore.note, action.tagId)
					break
			}
		})
	}
}
