import QtQuick 2.0
import com.github.galymzhan 0.1

QtObject {
	property var dispatcher

	function search(query) {
		dispatcher.dispatch({
			type: Actions.SEARCH_NOTE,
			query: query
		})
	}

	function select(index) {
		dispatcher.dispatch({
			type: Actions.SELECT_NOTE,
			index: index
		})
	}

	function create() {
		dispatcher.dispatch({
			type: Actions.CREATE_NOTE
		})
	}

	function update(content) {
		dispatcher.dispatch({
			type: Actions.UPDATE_NOTE,
			content: content
		})
	}

	function persist() {
		dispatcher.dispatch({
			type: Actions.PERSIST_NOTE
		})
	}

	function deleteNote() {
		dispatcher.dispatch({
			type: Actions.DELETE_NOTE
		})
	}
}
