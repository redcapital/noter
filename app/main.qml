import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.2
import com.github.galymzhan 0.1

ApplicationWindow {
	id: mainWindow
	visible: true
	width: 800
	height: 600

	Component.onCompleted: {
		//mainWindow.createOrOpenDatabase("file:///Users/galym/projects/opa.ndb", true)
	}

	function createOrOpenDatabase(filepath, isOpening) {
		if (repository.connect(filepath, isOpening)) {
			startupLayout.visible = false
			mainLayout.init()
		} else {
			showError(repository.getLastError())
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
		property Note currentNote

		Timer {
			interval: 200
			id: saveTimer
			onTriggered: mainLayout.saveNote()
		}

		function init() {
			noteListModel.query('')
			noteListView.currentIndex = -1
			mainLayout.currentNote = null
			editor.text = ''
			mainLayout.visible = true
		}

		function loadNote(note) {
			console.log('loaded', note.id)
			saveTimer.stop()
			saveNote()
			mainLayout.currentNote = null
			editor.text = note.content
			mainLayout.currentNote = note
		}

		function unloadNote() {
			saveTimer.stop()
			saveNote()
			noteListView.currentIndex = -1
			mainLayout.currentNote = null
			editor.text = ''
		}

		function saveNote(fromtimer) {
			if (mainLayout.currentNote) {
				console.log('saving', mainLayout.currentNote.id)
				repository.updateNote(mainLayout.currentNote)
			}
		}

		function createNote() {
			if (repository.createNote()) {
				unloadNote()
				noteListView.currentIndex = 0
				editor.focus = true
			}
		}

		function deleteNote() {
			saveTimer.stop()
			noteListView.currentIndex = -1
			if (mainLayout.currentNote) {
				repository.deleteNote(mainLayout.currentNote)
			}
			mainLayout.currentNote = null
			editor.text = ''
		}

		function formatDateTime(timestamp) {
			return new Date(timestamp * 1000).toLocaleString()
		}

		function formatMoment(timestamp) {
			var diff = Date.now() / 1000 - timestamp
			if (diff < 60) return 'moments ago';
			if (diff < 120) return '1 minute ago';
			if (diff < 3600) return Math.floor(diff / 60) + ' minutes ago'
			if (diff < 7200) return '1 hour ago';
			if (diff < 86400) return Math.floor(diff / 3600) + ' hours ago'
			var days = Math.floor(diff / 86400)
			if (days === 1) return 'Yesterday'
			if (days < 7) return days + ' days ago'
			if (days === 7) return '1 week ago'
			if (days < 31) return Math.ceil(days / 7) + ' weeks ago'
			if (days === 31) return '1 month ago'
			if (days < 365) return Math.ceil(days / 30) + ' months ago'
			return mainLayout.formatDateTime(timestamp)
		}

		Item {
			Layout.minimumWidth: 230
			Layout.maximumWidth: parent.width * 0.4

			Timer {
				interval: 200
				id: searchTimer
				onTriggered: {
					mainLayout.unloadNote()
					noteListModel.query(searchField.text)
				}
			}

			TextField {
				id: searchField
				anchors.margins: 10
				anchors.top: parent.top
				anchors.left: parent.left
				anchors.right: parent.right

				onTextChanged: {
					searchTimer.restart()
				}
			}

			ScrollView {
				anchors.topMargin: 10
				anchors.bottomMargin: 10
				anchors.top: searchField.bottom
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.bottom: parent.bottom

				style: ScrollViewStyle {
					transientScrollBars: true
					scrollToClickedPosition: true
					minimumHandleLength: 20
				}

				ListView {
					id: noteListView
					clip: true
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
							Label {
								elide: Text.ElideRight
								width: parent.width
								text: title.length ? title : '<empty>'
								font.weight: Font.DemiBold
								font.italic: title.length === 0
							}
							Label {
								elide: Text.ElideRight
								width: parent.width
								text: mainLayout.formatMoment(updatedAt)
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
						if (currentIndex >= 0) {
							mainLayout.loadNote(model.get(currentIndex))
						}
					}
				}
			}
		}


		ColumnLayout {
			spacing: 5

			RowLayout {
				Layout.fillWidth: true

				MessageDialog {
					id: confirmationDialog
					title: 'Confirmation'
					text: 'Are you sure?'
					icon: StandardIcon.Question
					standardButtons: StandardButton.Yes | StandardButton.No
					onYes: mainLayout.deleteNote()
				}

				Button {
					text: 'Create'
					onClicked: mainLayout.createNote()
				}

				Label {
					enabled: mainLayout.currentNote
					text: mainLayout.currentNote ? mainLayout.formatDateTime(mainLayout.currentNote.createdAt) : ''
				}

				Button {
					text: 'Delete'
					enabled: mainLayout.currentNote
					onClicked: confirmationDialog.open()
				}

				Item {
					Layout.fillWidth: true

					Button {
						text: 'Cog'
						anchors.rightMargin: 5
						anchors.right: parent.right
						anchors.verticalCenter: parent.verticalCenter

						menu: Menu {
							MenuItem {
								action: createDatabase
							}

							MenuItem {
								action: openDatabase
							}

							MenuItem {
								text: 'Database settings'
							}
						}
					}
				}
			}

			TextArea {
				id: editor
				Layout.fillWidth: true
				Layout.fillHeight: true

				onTextChanged: {
					if (mainLayout.currentNote) {
						saveTimer.restart()
					}
				}

				Binding {
					target: mainLayout.currentNote
					property: 'content'
					value: editor.text
					when: mainLayout.currentNote
				}
			}
		}
	}
}
