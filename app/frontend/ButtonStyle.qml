import QtQuick 2.3
import QtQuick.Controls.Styles 1.3 as Styles
import '.'

Styles.ButtonStyle {
	property color backgroundColor: Theme.primaryColor
	property int fontSize: Theme.baseFontSize
	property string fontFamily: Theme.fontFamily
	property color textColor: Theme.textColor
	property real horizontalPadding: fontSize
	property real verticalPadding: fontSize * 0.4

	function computeBackgroundColor(color) {
		if (control.pressed) return Qt.darker(color, 1.2)
		if (control.hovered) return Qt.lighter(color, 1.2)
		return color
	}

	background: Rectangle {
		color: {
			if (control.pressed) return Qt.darker(backgroundColor, 1.2)
			if (control.hovered) return Qt.lighter(backgroundColor, 1.2)
			return backgroundColor
		}
		radius: Theme.borderRadius

		Behavior on color {
			ColorAnimation { duration: 250; easing.type: Easing.Linear }
		}
	}

	label: Item {
		implicitWidth: label.width + horizontalPadding
		implicitHeight: label.height + verticalPadding

		Text {
			id: label
			anchors.centerIn: parent
			font.family: fontFamily
			font.pixelSize: fontSize
			color: textColor
			text: control.text
		}
	}
}
