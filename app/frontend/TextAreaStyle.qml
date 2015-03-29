import QtQuick 2.0
import QtQuick.Controls.Styles 1.3
import '.'

TextAreaStyle {
	backgroundColor: Theme.backgroundColor
	textColor: Theme.textColor
	font.family: Theme.fontFamily
	font.pixelSize: Theme.baseFontSize

	scrollToClickedPosition: true
	transientScrollBars: true

	handle: Item {
		implicitWidth: 10
		implicitHeight: 26
		Rectangle {
			anchors.fill: parent
			color: Qt.lighter(Theme.backgroundColor, 1.8)
			radius: parent.implicitWidth / 2
		}
	}

	scrollBarBackground: Item {
		implicitWidth: 5
		implicitHeight: 10
		MouseArea {
			id: mouse
			anchors.fill: parent
			hoverEnabled: true
		}
	}

	incrementControl: Item {}
	decrementControl: Item {}
}
