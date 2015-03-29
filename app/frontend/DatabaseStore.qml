import QtQuick 2.0
import Qt.labs.settings 1.0
import 'Flux.js' as Flux

QtObject {
	property Settings settings

	property string databaseFile: ''

	signal connected
	signal error(string message)

	Component.onCompleted: {
		Flux.dispatcher.register(function(action) {
			switch (action.type) {
				case Flux.Actions.CONNECT_DATABASE:
					if (repository.connect(action.filepath, action.isOpening)) {
						settings.lastDatabase = action.filepath
						connected()
					} else {
						settings.lastDatabase = ''
						error(repository.getLastError())
					}
					break
			}
		})
	}
}
