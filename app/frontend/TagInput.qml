import QtQuick 2.0 as Quick
import '.'

Quick.FocusScope {
	id: root
	implicitWidth: 400
	height: input.height + 20

	property string separator: ','
	property int triggerAutocompleteAfter: 1
	property alias model: tagList
	property alias enabled: input.enabled

	property var createTag: null
	property var autocompleteTag: null

	signal tagged(int tagId)
	signal untagged(int tagId)


	Quick.Rectangle {
		anchors.fill: parent
		color: '#fff'
		radius: Theme.borderRadius
		border.width: 1
		border.color: root.activeFocus ? Theme.primaryColor : 'transparent'

		Quick.Flickable {
			id: flickable
			anchors.fill: parent
			anchors.leftMargin: 5
			anchors.rightMargin: 5
			contentWidth: items.width + input.width
			flickableDirection: Quick.Flickable.HorizontalFlick
			boundsBehavior: Quick.Flickable.StopAtBounds
			clip: true

			Quick.Row {
				id: items
				anchors.left: parent.left
				anchors.verticalCenter: parent.verticalCenter
				spacing: 5

				Quick.Repeater {
					model: Quick.ListModel {
						id: tagList
						property var _tagIds: []

						function reload(tags) {
							clear()
							_tagIds = []
							for (var i = 0; i < tags.length; i++) {
								_tagIds.push(tags[i].tagId)
								append(tags[i])
							}
						}

						function _appendIfNotPresent(tag) {
							for (var i = 0; i < count; i++) {
								if (get(i).tagId === tag.tagId) {
									return false
								}
							}
							_tagIds.push(tag.tagId)
							append(tag)
							return true
						}

						function add(name) {
							if (!createTag) {
								console.warn("createTag callback not set")
								return
							}
							var tag = createTag(name)
							if (tag) {
								if (_appendIfNotPresent(tag)) {
									tagged(tag.tagId)
								}
							}
						}

						function select(tagId, name) {
							if ( _appendIfNotPresent({ tagId: tagId, name: name })) {
								tagged(tagId)
							}
						}

						function unselect(index) {
							if (index >= 0 && index < count) {
								var tagId = get(index).tagId, i = _tagIds.indexOf(tagId)
								remove(index, 1)
								_tagIds.splice(i, 1)
								untagged(tagId)
							}
						}

						function autocomplete(name) {
							if (!autocompleteTag) {
								console.warn('autocompleteTag callback not set')
								return
							}
							return autocompleteTag(name, _tagIds)
						}
					}

					delegate: Quick.Rectangle {
						width: tagIcon.width + item.width + remove.width + 30
						height: item.height + 10
						radius: Theme.borderRadius
						color: Theme.primaryColor

						Quick.Text {
							id: tagIcon
							text: Awesome.fa_tag
							color: Theme.textColor
							font.family: 'FontAwesome'
							font.pixelSize: Theme.baseFontSize * 0.8
							anchors.right: item.left
							anchors.verticalCenter: parent.verticalCenter
							anchors.rightMargin: 5
							textFormat: Quick.TextEdit.PlainText
						}

						Quick.Text {
							id: item
							text: name
							color: Theme.textColor
							font.family: Theme.fontFamily
							font.pixelSize: Theme.baseFontSize
							anchors.centerIn: parent
							textFormat: Quick.TextEdit.PlainText
						}

						Quick.MouseArea {
							id: hoverArea
							anchors.fill: parent
							hoverEnabled: true
						}

						Quick.Text {
							id: remove
							visible: hoverArea.containsMouse
							color: Theme.textColor
							text: Awesome.fa_times
							font.family: 'FontAwesome'
							font.pixelSize: Theme.baseFontSize
							anchors.right: parent.right
							anchors.verticalCenter: parent.verticalCenter
							anchors.rightMargin: 5
							textFormat: Quick.TextEdit.PlainText

							Quick.MouseArea {
								anchors.fill: parent
								onClicked:  tagList.unselect(index)
							}
						}
					}
				}

				Quick.TextInput {
					id: input
					anchors.verticalCenter: parent.verticalCenter
					width: Math.max(contentWidth, 150)
					color: '#000'
					font.family: Theme.fontFamily
					font.pixelSize: Theme.baseFontSize
					focus: true
					property bool _changedByUser: true

					onTextChanged: {
						// Make sure the textinput is visible inside the flickable
						var diff = (x + width) - (flickable.contentX + flickable.width)
						if (diff > 0) {
							flickable.contentX += diff
						}

						// Exit early if the change wasn't initiated by a user
						if (!_changedByUser) {
							_changedByUser = true
							return
						}

						var separatorPos = text.indexOf(separator)
						if (separatorPos >= 0) {
							tagList.add(text.substring(0, separatorPos))
							updateText('')
							autocompleteBox.visible = false
							return
						}

						if (text.length >= triggerAutocompleteAfter) {
							var results = tagList.autocomplete(text)
							if (results.length > 0) {
								autocompleteBox.show(results)
								return
							}
						}
						autocompleteBox.visible = false
					}

					function updateText(newText) {
						_changedByUser = false
						text = newText
					}

					Quick.Keys.onPressed: {
						if (event.key === Qt.Key_Backspace && text.length === 0) {
							tagList.unselect(tagList.count - 1)
							return
						}
						if (autocompleteBox.visible && (event.key === Qt.Key_Up || event.key === Qt.Key_Down)) {
							var index = autocompleteRepeater.selectedIndex + (event.key === Qt.Key_Up ? -1 : 1)
							if (index < 0) index = autocompleteRepeater.model.length - 1
							if (index === autocompleteRepeater.model.length) index = 0
							autocompleteRepeater.selectedIndex = index
						}
					}

					onAccepted: {
						tagList.add(text)
						autocompleteBox.visible = false
						updateText('')
					}
				}
			}
		}

		Quick.Rectangle {
			id: autocompleteBox
			visible: false
			color: parent.color
			width: autocompleteItems.width
			height: autocompleteItems.height + radius * 2
			radius: Theme.borderRadius
			x: input.x - flickable.contentX
			anchors.top: parent.bottom

			Quick.Column {
				anchors.verticalCenter: parent.verticalCenter
				id: autocompleteItems

				Quick.Repeater {
					id: autocompleteRepeater
					property int selectedIndex: -1

					delegate: Quick.Rectangle {
						width: autocompleteItem.width + 10
						height: autocompleteItem.height + 10
						color: index === autocompleteRepeater.selectedIndex ? Theme.backgroundColor : 'transparent'

						Quick.Text {
							id: autocompleteItem
							anchors.centerIn: parent
							text: modelData.name
							color: index === autocompleteRepeater.selectedIndex ? Theme.textColor : '#000'
							width: 200
							elide: Quick.Text.ElideRight
							textFormat: Quick.TextEdit.PlainText
						}

						Quick.MouseArea {
							anchors.fill: parent
							hoverEnabled: true
							onClicked: {
								tagList.select(modelData.tagId, modelData.name)
								input.updateText('')
								autocompleteBox.visible = false
							}
							onEntered: autocompleteRepeater.selectedIndex = index
						}
					}

					onSelectedIndexChanged: if (selectedIndex > -1) input.updateText(model[selectedIndex].name)
				}
			}

			function show(results) {
				autocompleteRepeater.selectedIndex = -1
				autocompleteRepeater.model = results
				visible = true
			}
		}
	}

	Quick.MouseArea {
		anchors.fill: parent
		onClicked: if (root.enabled) root.forceActiveFocus()
	}
}
