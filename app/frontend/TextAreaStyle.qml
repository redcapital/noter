import QtQuick 2.0 as Quick
import QtQuick.Controls.Styles 1.3 as Styles
import '.'

Styles.TextAreaStyle {
	backgroundColor: Theme.editorConfig.backgroundColor
	textColor: Theme.editorConfig.textColor
	font.family: Theme.editorConfig.fontFamily
	font.pixelSize: Theme.editorConfig.fontSize

	scrollToClickedPosition: true
	transientScrollBars: true

	handle: Quick.Item {
		implicitWidth: 10
		implicitHeight: 26
		Quick.Rectangle {
			anchors.fill: parent
			color: Qt.lighter(Theme.backgroundColor, 1.8)
			radius: parent.implicitWidth / 2
		}
	}

	scrollBarBackground: Quick.Item {
		implicitWidth: 5
		implicitHeight: 10
		Quick.MouseArea {
			id: mouse
			anchors.fill: parent
			hoverEnabled: true
		}
	}

	incrementControl: Quick.Item {}
	decrementControl: Quick.Item {}
}
