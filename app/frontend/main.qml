import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.2
import Qt.labs.settings 1.0
import com.github.galymzhan 0.1
import 'Flux.js' as Flux

ApplicationWindow {
	id: mainWindow
	visible: true
	width: 800
	height: 600

	property var dispatcher

	Settings {
		id: settings
		property string lastDatabase
	}

	FontLoader {
		// id: fontUbuntu
		source: 'fonts/UbuntuMono-R.ttf'
	}
	FontLoader {
		source: 'fonts/UbuntuMono-RI.ttf'
	}
	FontLoader {
		source: 'fonts/UbuntuMono-B.ttf'
	}
	FontLoader {
		source: 'fonts/UbuntuMono-BI.ttf'
	}

	DatabaseStore {
		id: databaseStore
		settings: settings

		onConnected: {
			mainLayout.init()
		}

		onError: {
			showError(message)
			if (!startupLayout.visible && !mainLayout.visible) {
				startupLayout.visible = true
			}
		}
	}

	NoteStore {
		id: noteStore
	}

	DatabaseActions {
		id: databaseActions
		dispatcher: mainWindow.dispatcher
	}

	NoteActions {
		id: noteActions
		dispatcher: mainWindow.dispatcher
	}

	Component.onCompleted: {
		dispatcher = new Flux.Dispatcher()
		databaseStore.init(dispatcher)
		noteStore.init(dispatcher)
		if (settings.lastDatabase) {
			databaseActions.connectDatabase(settings.lastDatabase, true)
		} else {
			startupLayout.visible = true
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
		onAccepted: databaseActions.connectDatabase(this.fileUrl, true)
	}

	FileDialog {
		id: createFileDialog
		selectFolder: false
		selectMultiple: false
		selectExisting: false
		nameFilters: ['Noter database (*.ndb)']
		onAccepted: databaseActions.connectDatabase(this.fileUrl, false)
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
		visible: false
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

		function init() {
			mainLayout.visible = true
			noteActions.search('')
		}

		function formatDateTime(timestamp) {
			return new Date(timestamp * 1000).toLocaleString()
		}

		function formatMoment(timestamp, now) {
			// now is calculated once per 10-seconds, so it may be a bit behind
			var diff = Math.max(0, now - timestamp)
			if (diff < 60) return 'moments ago'
			if (diff < 120) return '1 minute ago'
			if (diff < 3600) return Math.floor(diff / 60) + ' minutes ago'
			if (diff < 7200) return '1 hour ago'
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
				interval: 100
				id: searchTimer
				onTriggered: {
					noteActions.search(searchField.text)
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
					model: noteStore.model

					Connections {
						target: noteStore
						onIndexChanged: noteListView.currentIndex = newIndex
					}

					Timer {
						id: updatedTimer
						property int timestamp
						interval: 10000
						running: true
						repeat: true
						triggeredOnStart: true
						onTriggered: timestamp = Date.now() / 1000
					}

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
								id: updatedAtLabel
								elide: Text.ElideRight
								width: parent.width
								text: mainLayout.formatMoment(updatedAt, updatedTimer.timestamp)
								font.pointSize: 12
								color: '#555'
							}
						}

						MouseArea {
							anchors.fill: parent
							onClicked: {
								noteActions.select(index)
								editor.focus = true
							}
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
					id: deleteConfirmationDialog
					title: 'You are about to delete a note'
					text: 'Are you sure?'
					icon: StandardIcon.Question
					standardButtons: StandardButton.Yes | StandardButton.No
					onYes: noteActions.deleteNote()
				}

				Button {
					text: 'Create'
					onClicked: noteActions.create()
				}

				Label {
					enabled: noteStore.note
					text: noteStore.note ? mainLayout.formatDateTime(noteStore.note.createdAt) : ''
				}

				Button {
					text: 'Delete'
					enabled: noteStore.note
					onClicked: deleteConfirmationDialog.open()
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
				text: noteStore.note ? noteStore.note.content : '<select a note>'
				textMargin: 6
				enabled: noteStore.note

				Component.onCompleted: {
					textAreaBackend.setDocument(textDocument)
				}

				Timer {
					interval: 200
					id: persistTimer
					onTriggered: if (noteStore.note) noteActions.persist()
				}

				Connections {
					target: noteStore
					onNoteChanged: persistTimer.stop()
				}

				onTextChanged: {
					if (noteStore.note) {
						noteActions.update(text)
						persistTimer.restart()
					}
				}

				style: TextAreaStyle {
					backgroundColor: '#263238'
					textColor: '#f8f8f2'
					selectedTextColor: '#37474f'
					font.family: 'Ubuntu Mono'
					font.pointSize: 14
				}
			}
		}
	}
}
