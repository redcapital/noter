import QtQuick 2.3
import QtQuick.Controls.Styles 1.3 as Styles
import '.'

Styles.TextFieldStyle {
	background: Rectangle {
		radius: Theme.borderRadius
		color: '#fff'
		border.width: 1
		border.color: control.activeFocus ? Theme.primaryColor : 'transparent'
		implicitHeight: font.pixelSize * 2
	}
	textColor: '#000'
	placeholderTextColor: '#bdc3c7'
	font.family: Theme.fontFamily
	font.pixelSize: Theme.baseFontSize
}
