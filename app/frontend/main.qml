import QtQuick 2.2 as Quick
import QtQuick.Controls 1.2 as Controls
import QtQuick.Dialogs 1.2
import Qt.labs.settings 1.0
import 'Flux.js' as Flux
import 'Moment.js' as Moment
import '.'

Controls.ApplicationWindow {
	id: mainWindow
	visible: true
	width: 800
	height: 600
	color: Theme.backgroundColor
	title: {
		var title = databaseStore.databaseFile
		if (title.substring(0, 7) === 'file://') title = title.substring(7)
		if (title) title += ' - '
		title += 'Noter'
		return title
	}

	Settings {
		id: settings
		property string lastDatabase
	}

	Quick.FontLoader { source: 'fonts/UbuntuMono-R.ttf' }
	Quick.FontLoader { source: 'fonts/UbuntuMono-RI.ttf' }
	Quick.FontLoader { source: 'fonts/UbuntuMono-B.ttf' }
	Quick.FontLoader { source: 'fonts/UbuntuMono-BI.ttf' }
	Quick.FontLoader { source: 'fonts/Lato-Regular.ttf' }
	Quick.FontLoader { source: 'fonts/Lato-Bold.ttf' }
	Quick.FontLoader { source: 'fonts/Lato-BoldItalic.ttf' }
	Quick.FontLoader { source: 'fonts/Lato-Italic.ttf' }
	Quick.FontLoader { source: 'fonts/fontawesome-webfont.ttf' }

	DatabaseStore {
		id: databaseStore
		settings: settings

		onConnected: {
			startupPage.visible = false
			mainPage.show()
		}

		onError: {
			mainPage.visible = false
			startupPage.visible = true
			showError(message)
		}
	}

	NoteStore {
		id: noteStore
	}

	UiStore {
		id: uiStore
	}

	TagStore {
		id: tagStore
	}

	Quick.Component.onCompleted: {
		if (settings.lastDatabase) {
			Flux.DatabaseActions.connectDatabase(settings.lastDatabase, true)
		} else {
			startupPage.visible = true
		}
	}

	function showError(message) {
		errorDialog.title = 'Error'
		errorDialog.icon = StandardIcon.Critical
		errorDialog.text = message
		errorDialog.open()
	}

	MessageDialog {
		id: errorDialog
	}

	FileDialog {
		id: openFileDialog
		selectFolder: false
		selectMultiple: false
		selectExisting: true
		nameFilters: ['Noter database (*.ndb)']
		onAccepted: Flux.DatabaseActions.connectDatabase(this.fileUrl, true)
	}

	FileDialog {
		id: createFileDialog
		selectFolder: false
		selectMultiple: false
		selectExisting: false
		nameFilters: ['Noter database (*.ndb)']
		onAccepted: Flux.DatabaseActions.connectDatabase(this.fileUrl, false)
	}

	Controls.Action {
		id: openDatabase
		text: qsTr('Open database')
		onTriggered: openFileDialog.open()
	}

	Controls.Action {
		id: createDatabase
		text: qsTr('Create new database')
		onTriggered: createFileDialog.open()
	}

	StartupPage {
		id: startupPage
	}

	MainPage {
		id: mainPage
		anchors.fill: parent
	}

	Quick.Timer {
		id: timer10k
		property int ticks: 0
		interval: 10000
		running: true
		repeat: true
		onTriggered: ++ticks
	}
}
