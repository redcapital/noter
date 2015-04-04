import QtQuick 2.2 as Quick
import QtQuick.Controls 1.2 as Controls
import 'Flux.js' as Flux
import '.'

Quick.Rectangle {
	implicitWidth: 65
	implicitHeight: 400
	color: '#BDC3C7'

	Quick.Column {
		anchors.centerIn: parent
		spacing: 15

		Controls.Button {
			text: Awesome.fa_plus_circle
			style: IconButtonStyle {
				inverted: true
			}
			onClicked: Flux.NoteActions.create()
		}

		Controls.Button {
			onClicked: {
				if (uiStore.searchPanelOpen) {
					Flux.UiActions.closeSearchPanel()
				} else {
					Flux.UiActions.openSearchPanel()
				}
			}
			text: Awesome.fa_search
			style: IconButtonStyle {
				inverted: true
				textColor: uiStore.searchPanelOpen ? Theme.primaryColor : computeBackgroundColor(Theme.backgroundColor)
			}
		}
	}

	Controls.Button {
		anchors.top: parent.top
		anchors.topMargin: 10
		anchors.horizontalCenter: parent.horizontalCenter
		text: Awesome.fa_bars
		style: IconButtonStyle {
			inverted: true
			fontSize: Theme.baseFontSize * 1.8
		}
		menu: Controls.Menu {
			Controls.MenuItem {
				action: createDatabase
			}
			Controls.MenuItem {
				action: openDatabase
			}
			Controls.MenuItem {
				text: 'About'
			}
			Controls.MenuItem {
				text: 'Quit'
				onTriggered: Qt.quit()
			}
		}
	}
}
