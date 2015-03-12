import QtQuick 2.0
import com.github.galymzhan 0.1

QtObject {
	property var dispatcher

	function connectDatabase(filepath, isOpening) {
		dispatcher.dispatch({
			type: Actions.CONNECT_DATABASE,
			filepath: filepath,
			isOpening: isOpening
		});
	}
}
