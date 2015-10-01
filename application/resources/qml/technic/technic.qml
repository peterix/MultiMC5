import QtQuick 2.3
import "qrc:///qml/" as MultiMC

Item
{
	visible: true
	anchors.fill: parent
	id: root
	property int minWidth: 880;
	property int minHeight: 520;
	property int currentPack: packListView.currentIndex;
	property string currentVersion: versionListView.model.get(versionListView.currentIndex).name;
	property string instanceName: textLabel.text;
	SystemPalette
	{
		id: myPalette; colorGroup: SystemPalette.Active
	}
	property color shadowColor: "#aa000000"
	property color frameColor: "#77ffffff"
	property color highlightColor: myPalette.highlight
	property color highlightedTextColor: myPalette.highlightedText

	focus: true


	Keys.onPressed:
	{
		if (event.key === Qt.Key_Escape)
		{
			dialog.reject()
			event.accepted = true;
		}
	}

	// enable for moving window by dragging anywhere
	/*
	MouseArea
	{
		anchors.fill: parent
		property int px
		property int py

		onPressed:
		{
			dialog.startDragging();
		}
		onPositionChanged: {
			dialog.updateDragging();
		}
	}
	*/

	Component
	{
		id: packDelegate
		Rectangle
		{
			height: 120
			width: 190
			radius: 5
			color: "#00000000"
			Image
			{
				id: delegateImage
				cache: true
				anchors.centerIn: parent
				source: logo
				height: 110
				width: 180
				scale: packListView.currentIndex === index ? 1.0 : 0.9
				fillMode: Image.PreserveAspectCrop
				Behavior on scale {
					NumberAnimation { duration: 200 }
				}
			}
			MouseArea
			{
				anchors.fill:  parent
				propagateComposedEvents: true
				onClicked:
				{
					if(packListView.currentIndex === index)
					{
						return;
					}
					var labelText = textLabel.text;
					var modelText = packModel.get(packListView.currentIndex).display_name;
					console.log ("label:", textLabel.text);
					console.log ("model:",packModel.get(packListView.currentIndex).display_name);

					if(labelText == modelText || modelText == undefined || labelText.length == 0)
					{
						textLabel.text = display_name
					}
					packListView.currentIndex = index;
					// this actually resets the version model
					dialog.setCurrent(index);
					if(recommended != -1)
						versionListView.currentIndex = recommended
					else if (latest != -1)
						versionListView.currentIndex = latest
					else
						versionListView.currentIndex = 0
					console.log ("currentIndex", packListView.currentIndex, "currentPack", currentPack);
				}
			}
		}
	}

	Component
	{
		id: packViewHighlight
		Rectangle
		{
			color: highlightColor
			height: 120
			width: 190
			radius: 8
			y: packListView.currentItem.y
			Behavior on y {
				SpringAnimation {
					spring: 3
					damping: 0.2
				}
			}
		}
	}

	Rectangle
	{
		id: packList
		width: 190
		anchors.top: parent.top
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		anchors.rightMargin: 5
		color: shadowColor
		ListView
		{
			id: packListView
			focus: true
			anchors.fill: parent
			model: packModel
			delegate: packDelegate
			highlight: packViewHighlight
			highlightFollowsCurrentItem: false
			boundsBehavior: Flickable.DragOverBounds
		}
	}

	Rectangle
	{
		id: packLabel
		anchors.bottom: parent.bottom
		anchors.left: packList.right
		anchors.right: install.left
		anchors.margins: 10
		height: 64
		radius: 8
		color: frameColor
		border
		{
			color: "black"
			width: 1
		}
		Rectangle
		{
			border
			{
				color: "black"
				width: 1
			}
			anchors.fill: parent
			anchors.margins: 8
			color: "#ffffffff"
			TextInput
			{
				id: textLabel
				selectByMouse: true
				clip: true
				focus: true
				verticalAlignment: TextInput.AlignVCenter
				anchors.fill: parent
				anchors.margins: 8
				font.pixelSize: 24
				selectionColor: highlightColor
				selectedTextColor: highlightedTextColor
			}
		}
	}

	Component
	{
		id: versionDelegate
		Item
		{
			id: item
			width: parent.width
			height:  versionRow.height
			function altColor(i)
			{
				var colors = [ "#aadedede", "#aacecece" ];
				return colors[i];
			}
			Rectangle
			{
				width:  parent.width
				height: parent.height
				color: versionListView.currentIndex === index ? highlightColor : altColor(index%2)
				MouseArea
				{
					anchors.fill:  parent
					propagateComposedEvents: true
					onClicked:
					{
						versionListView.currentIndex = index;
						mouse.accepted = true
					}
				}
			}
			Row
			{
				id: versionRow
				Item
				{
					width: 5
					height: 1
				}
				Text
				{
					text: name
					width: item.width * 0.7
					font.pixelSize: Math.min(root.width / 44, root.height / 26)
					color: versionListView.currentIndex === index ? highlightedTextColor : "black"
				}
				Text
				{
					text: latest ? qsTr("Latest") : (recommended ? qsTr("Recommended") : "")
					width: item.width * 0.3
					font.pixelSize: Math.min(root.width / 44, root.height / 26)
					color: versionListView.currentIndex === index ? highlightedTextColor : "black"
				}
			}
		}
	}

	Rectangle
	{
		id: versionListFrame
		anchors
		{
			top: parent.top
			bottom: packLabel.top
			left: packList.right
			right: install.left
			margins: 10
		}
		radius: 8
		color: frameColor
		clip: true
		border
		{
			color: "black"
			width: 1
		}

		Rectangle
		{
			color: "#00000000"
			clip: true
			border
			{
				color: "black"
				width: 1
			}
			anchors
			{
				fill: parent
				margins: 8
			}
			ListView
			{
				id: versionListView
				focus: true
				clip: true
				anchors
				{
					fill: parent
					margins: 1
				}
				model: versionModel
				delegate: versionDelegate
				boundsBehavior: Flickable.DragOverBounds
			}
			/*
			Rectangle
			{
				visible: versionListView.visibleArea.heightRatio < 1
				anchors.right: versionListView.right
				y: versionListView.visibleArea.yPosition * versionListView.height + 1
				width: 3
				height: versionListView.visibleArea.heightRatio * versionListView.height
				color: "black"
				z: 2
			}
			*/
			MultiMC.ScrollBar
			{
				flickable: versionListView
			}
		}
	}

	Image
	{
		id: background
		cache: true
		anchors.fill: parent
		source: packModel.get(packListView.currentIndex).background
		fillMode: Image.PreserveAspectCrop
		z: -1
	}

	Image
	{
		anchors.fill: parent
		source: "default_background.jpg";
		visible: background.status !== Image.Ready;
		z: -1
	}

	Image
	{
		id: close
		cache: true
		anchors.right: parent.right
		anchors.top: parent.top
		anchors.margins: 10
		source: "close.svg"
		scale: closeMouseArea.containsMouse ? 1.2 : 1.0
		MouseArea
		{
			hoverEnabled: true
			id: closeMouseArea
			anchors.fill: parent
			onClicked: dialog.reject();
		}
	}

	Image
	{
		id: install
		cache: true
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.margins: 10
		source: "install.svg"
		scale: installMouseArea.containsMouse ? 1.2 : 1.0
		MouseArea
		{
			hoverEnabled: true
			id: installMouseArea
			anchors.fill: parent
			onClicked: dialog.accept();
		}
	}
}
