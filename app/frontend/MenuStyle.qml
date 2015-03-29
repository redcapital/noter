import QtQuick 2.0
import QtQuick.Controls.Styles 1.3 as Styles

Styles.MenuStyle {
	frame: Rectangle {
		color: 'red'
	}
	itemDelegate.background: Rectangle {
			color: 'yellow'
			implicitWidth: 50
			implicitHeight: 50

	}
}
