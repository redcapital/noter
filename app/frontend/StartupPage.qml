import QtQuick 2.0 as Quick
import '.'

Quick.Column {
	visible: false
	anchors.centerIn: parent
	spacing: 15

	Text {
		font.pixelSize: Theme.baseFontSize * 2
		anchors.horizontalCenter: parent.horizontalCenter
		text: qsTr('Welcome to Noter!')
	}
	Button {
		action: openDatabase
		anchors.horizontalCenter: parent.horizontalCenter
		width: 200
	}
	Button {
		action: createDatabase
		anchors.horizontalCenter: parent.horizontalCenter
		width: 200
	}
}
