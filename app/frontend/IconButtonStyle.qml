import QtQuick 2.0 as Quick
import '.'

ButtonStyle {
	property bool inverted: false
	fontFamily: 'FontAwesome'
	fontSize: Theme.baseFontSize * 2.5
	horizontalPadding: 8
	verticalPadding: 0

	backgroundColor: inverted ? 'transparent' : Theme.primaryColor

	textColor: {
		if (!inverted) return Theme.textColor
		return computeBackgroundColor(Theme.backgroundColor)
	}
}
