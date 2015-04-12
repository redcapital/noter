import QtQuick 2.0 as Quick
import 'Flux.js' as Flux
import 'Moment.js' as Moment
import '.'

Quick.Rectangle {
	implicitWidth: 260
	implicitHeight: 400
	color: Qt.lighter(Theme.backgroundColor, 1.2)

	Quick.Timer {
		id: timer
		interval: 100
		onTriggered: {
			Flux.NoteActions.search(search.text)
		}
	}

	TextField {
		id: search
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: 10
		placeholderText: qsTr('Search')

		onTextChanged: {
			timer.restart()
		}
	}

	Quick.ListView {
		id: view
		anchors.top: search.bottom
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.topMargin: 10
		clip: true
		boundsBehavior: Quick.Flickable.StopAtBounds

		model: noteStore.model

		delegate: Quick.Rectangle {
			property bool isActive: (noteStore.note && noteStore.note.getId() === id)
			width: parent.width
			implicitHeight: 20
			height: elementDetails.height + 14
			color: isActive ? Theme.backgroundColor : 'transparent'

			Quick.Column {
				id: elementDetails
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.leftMargin: 10
				anchors.rightMargin: 10
				anchors.top: parent.top
				anchors.topMargin: 5
				spacing: 3

				Quick.Text {
					elide: Quick.Text.ElideRight
					width: parent.width
					text: {
						timer10k.ticks // accessing ticks will force the binding to update when ticks change
						return Moment.ago(updatedAt)
					}
					font.family: Theme.fontFamily
					font.pixelSize: Theme.baseFontSize * 0.9
					color: Theme.secondaryColor
					textFormat: Quick.TextEdit.PlainText
				}

				Quick.Text {
					elide: Quick.Text.ElideRight
					width: parent.width
					text: title.length ? title : '<empty>'
					font.family: Theme.fontFamily
					font.pixelSize: Theme.baseFontSize
					font.weight: Quick.Font.Bold
					font.italic: title.length === 0
					color: Theme.textColor
					textFormat: Quick.TextEdit.PlainText
				}
			}

			Quick.MouseArea {
				anchors.fill: parent
				onClicked: {
					Flux.NoteActions.select(id)
				}
			}
		}
	}
}
