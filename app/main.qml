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
		visible: false

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
		visible: true
		anchors.fill: parent

		Item {
			Layout.minimumWidth: 230
			Layout.maximumWidth: parent.width * 0.4

			Timer {
				interval: 200
				id: searchTimer
				onTriggered: {
					noteListModel.query(searchField.text)
				}
			}

			TextField {
				id: searchField
				anchors.margins: 10
				anchors.top: parent.top
				anchors.left: parent.left
				anchors.right: parent.right

				Keys.onPressed: {
					searchTimer.restart()
				}
			}

			ListView {
				id: noteListView
				anchors.topMargin: 10
				anchors.bottomMargin: 10
				anchors.top: searchField.bottom
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.bottom: parent.bottom
				boundsBehavior: Flickable.StopAtBounds
				model: noteListModel
				delegate: Rectangle {
					width: parent.width
					height: noteContent.height + 5
					color: ListView.isCurrentItem ? 'lightsteelblue' : 'transparent'

					Column {
						id: noteContent
						anchors.left: parent.left
						anchors.right: parent.right
						anchors.leftMargin: 10
						anchors.rightMargin: 10
						spacing: 5
						Text {
							elide: Text.ElideRight
							width: parent.width
							text: title
							font.weight: Font.DemiBold
						}
						Text {
							elide: Text.ElideRight
							width: parent.width
							text: updatedAt
							font.pointSize: 12
							color: '#555'
						}
					}
					MouseArea {
						anchors.fill: parent
						onClicked: {
							noteListView.currentIndex = index
						}
					}
				}

				onCurrentIndexChanged: {
					editor.text = model.get(currentIndex)
				}
			}
		}

		TextArea {
			id: editor

			onTextChanged: {
				console.log('asf')
			}
		}
	}
}
