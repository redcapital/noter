import QtQuick 2.0
import Qt.labs.settings 1.0
import com.github.galymzhan 0.1

QtObject {
	property var dispatcher
	property Settings settings

	property string databaseFile: ''

	signal connected
	signal error(string message)

	function init(_dispatcher) {
		dispatcher = _dispatcher
		var token = dispatcher.register(function(action) {
			switch (action.type) {
				case Actions.CONNECT_DATABASE:
					if (repository.connect(action.filepath, action.isOpening)) {
						settings.lastDatabase = action.filepath
						connected()
					} else {
						settings.lastDatabase = ''
						error(repository.getLastError())
					}
					break
			}
		});
	}
}
