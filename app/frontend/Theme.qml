import QtQuick 2.0

pragma Singleton

QtObject {
	id: theme

	property color primaryColor: '#D35400'
	property color secondaryColor: '#7F8C8D'

	property color backgroundColor: '#2C3E50'
	property color textColor: '#fff'

	property string fontFamily: 'Lato'
	property int baseFontSize: 14

	property int borderRadius: 4
}
