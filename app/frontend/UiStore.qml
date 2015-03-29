import QtQuick 2.0 as Quick
import 'Flux.js' as Flux

Quick.QtObject {
	property bool searchPanelOpen: true

	Quick.Component.onCompleted: {
		Flux.dispatcher.register(function(action) {
			switch (action.type) {
				case Flux.Actions.OPEN_SEARCH_PANEL:
					searchPanelOpen = true
					break

				case Flux.Actions.CLOSE_SEARCH_PANEL:
					searchPanelOpen = false
					break
			}
		})
	}
}
