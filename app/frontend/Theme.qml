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

	// Text Editor
	property var editorConfig: ({
		fontFamily: 'Ubuntu Mono',
		fontSize: 15,
		textColor: '#fff',
		backgroundColor: '#2C3E50',

		header: {
			color: '#BDC3C7',
			bold: true
		},
		list: {
			color: '#F1C40F'
		},
		link: {
			color: '#3498DB'
		},
		image: {
			color: '#3498DB'
		},
		code: {
			color: '#95A5A6',
			fontFamily: 'Ubuntu Mono'
		},
		emphasis: {
			color: '#BDC3C7',
			italic: true
		},
		strong: {
			color: '#BDC3C7',
			bold: true
		},
		blockquote: {
			color: '#ECF0F1'
		}
	})
}
