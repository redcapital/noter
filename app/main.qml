import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

ApplicationWindow {
	id: mainWindow
	visible: true
	width: 800
	height: 600

	function createOrOpenDatabase(filepath, isOpening) {
		if (!repository.connectDatabase(filepath, isOpening)) {
			showError(repository.getLastError())
		}
		startupLayout.visible = false
		mainLayout.visible = true
	}

	function showError(message) {
		messageDialog.title = 'Error'
		messageDialog.icon = StandardIcon.Critical
		messageDialog.text = message
		messageDialog.open()
	}

	MessageDialog {
		id: messageDialog
	}

	FileDialog {
		id: openFileDialog
		selectFolder: false
		selectMultiple: false
		selectExisting: true
		nameFilters: ['Noter database (*.ndb)']
		onAccepted: createOrOpenDatabase(this.fileUrl, true)
	}

	FileDialog {
		id: createFileDialog
		selectFolder: false
		selectMultiple: false
		selectExisting: false
		nameFilters: ['Noter database (*.ndb)']
		onAccepted: createOrOpenDatabase(this.fileUrl, false)
	}

	Action {
		id: openDatabase
		text: qsTr('Open database')
		onTriggered: openFileDialog.open()
	}

	Action {
		id: createDatabase
		text: qsTr('Create new database')
		onTriggered: createFileDialog.open()
	}

	GridLayout {
		id: startupLayout
		columns: 2
		rows: 2
		anchors.centerIn: parent

		Label {
			text: qsTr('Welcome to Noter!')
			Layout.columnSpan: 2
			Layout.alignment: Qt.AlignCenter
			font.pointSize: 20
		}

		Button {
			action: openDatabase
		}

		Button {
			action: createDatabase
		}
	}

	SplitView {
		id: mainLayout
		visible: false
		anchors.fill: parent

		Item {
			Layout.minimumWidth: 230
			Layout.maximumWidth: parent.width * 0.4

			ListView {
				anchors.fill: parent
				model: noteListModel
				delegate: Text { text: display }
			}
		}

		Rectangle {
			color: "#b56868"
		}
	}
}
