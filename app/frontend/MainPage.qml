import QtQuick 2.0 as Quick
import QtQml 2.2 as Qml
import QtQuick.Controls 1.2 as Controls
import QtQuick.Layouts 1.1 as Layouts
import QtQuick.Dialogs 1.2 as Dialogs
import 'Flux.js' as Flux
import '.'

Quick.Item {
	visible: false
	implicitWidth: 600
	implicitHeight: 400

	function show() {
		visible = true
		Flux.NoteActions.search('')
	}

	Dialogs.MessageDialog {
		id: deleteConfirmationDialog
		title: 'You are about to delete a note'
		text: 'Are you sure?'
		icon: Dialogs.StandardIcon.Question
		standardButtons: Dialogs.StandardButton.Yes | Dialogs.StandardButton.No
		onYes: Flux.NoteActions.deleteNote()
	}

	Gutter {
		id: gutter
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.bottom: parent.bottom
		width: 65
		z: 1
	}

	SearchPanel {
		id: listViewPanel
		anchors.top: parent.top
		anchors.bottom: parent.bottom
		width: 260
		x: uiStore.searchPanelOpen ? gutter.width : gutter.width - width
		visible: uiStore.searchPanelOpen

		Quick.Behavior on x {
			Quick.NumberAnimation { duration: 100; easing.type: Quick.Easing.Linear }
		}
	}

	Quick.Item {
		anchors.left: listViewPanel.right
		anchors.leftMargin: 10
		anchors.top: parent.top
		anchors.topMargin: 10
		anchors.right: parent.right
		anchors.rightMargin: 10
		anchors.bottom: parent.bottom

		Layouts.RowLayout {
			id: controlsBar
			anchors.top: parent.top
			anchors.left: parent.left
			anchors.right: parent.right

			Quick.Text {
				text: {
					if (!noteStore.note) return ''
					var date = new Date(noteStore.note.getCreatedAt() * 1000)
					return date.toLocaleDateString(Qt.locale(), 'dd MMM yyyy, dddd')
							+ "\n"
							+ date.toLocaleTimeString(Qt.locale(), Qml.Locale.ShortFormat)
				}

				font.family: Theme.fontFamily
				font.pixelSize: Theme.baseFontSize
				color: Theme.textColor
				font.weight: Quick.Font.Bold
				Layouts.Layout.fillWidth: true
				textFormat: Quick.TextEdit.PlainText
			}

			Controls.Button {
				text: Awesome.fa_trash_o
				enabled: noteStore.note
				style: IconButtonStyle {
					inverted: true
					textColor: Theme.secondaryColor
					fontSize: Theme.baseFontSize * 1.5
				}
				onClicked: deleteConfirmationDialog.open()
			}
		}

		TagInput {
			id: tagInput
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.top: controlsBar.bottom
			anchors.topMargin: 10
			enabled: noteStore.note
			z: 1

			createTag: tagStore.createTag

			autocompleteTag: tagStore.autocompleteTag

			onTagged: {
				Flux.TagActions.tag(tagId)
			}

			onUntagged: {
				Flux.TagActions.untag(tagId)
			}

			Quick.Connections {
				target: tagStore
				onTagListReloaded: tagInput.model.reload(tagList)
			}
		}

		Controls.TextArea {
			id: editor
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.top: tagInput.bottom
			anchors.topMargin: 20
			anchors.bottom: parent.bottom
			frameVisible: false
			textMargin: 0
			wrapMode: Quick.TextEdit.Wrap
			enabled: noteStore.note
			text: 'plz select a note'
			textFormat: Quick.TextEdit.PlainText

			Quick.Component.onCompleted: {
				textAreaBackend.setDocument(textDocument)
				textAreaBackend.setHighlightingRules(Theme.editorConfig)
			}

			style: TextAreaStyle {}

			Quick.Timer {
				interval: 200
				id: persistTimer
				onTriggered: if (noteStore.note) Flux.NoteActions.persist()
			}

			Quick.Connections {
				target: noteStore
				onNoteChanged: {
					// Store takes care of persisting if an active note changed
					persistTimer.stop()
					editor.text = noteStore.note ? noteStore.note.getContent() : 'plz select a note'
				}
			}

			onTextChanged: {
				if (noteStore.note) {
					Flux.NoteActions.update(text)
					persistTimer.restart()
				}
			}
		}
	}
}
