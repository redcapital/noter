import QtQuick 2.0
import Qt.labs.settings 1.0
import 'Flux.js' as Flux

QtObject {
	property string dispatchToken

	property Settings settings

	property string databaseFile: ''

	signal connected
	signal error(string message)

	Component.onCompleted: {
		dispatchToken = Flux.dispatcher.register(function(action) {
			switch (action.type) {
				case Flux.Actions.CONNECT_DATABASE:
					if (repository.connect(action.filepath, action.isOpening)) {
						settings.lastDatabase = databaseFile = action.filepath
						connected()
					} else {
						settings.lastDatabase = databaseFile = ''
						error(repository.getLastError())
					}
					break
			}
		})
	}
}
