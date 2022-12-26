/*
 * Copyright (C) 2022 - Ed Beroset <beroset@ieee.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.9
import org.asteroid.controls 1.0
import org.asteroid.utils 1.0
import QtQuick.VirtualKeyboard 2.1
import QtQuick.VirtualKeyboard.Settings 2.15

Item {
    id: root
    signal activated(string placename, string lat, string lon)
    property alias placename: locationName.text

    InputPanel {
        id: kbd
        anchors.centerIn: parent
        width: Dims.w(95)
    }

    Flickable {
        anchors.fill: parent
        contentHeight: mycol.implicitHeight
        boundsBehavior: Flickable.DragOverBounds
        flickableDirection: Flickable.HorizontalAndVerticalFlick
        anchors.margins: Dims.l(15)

        Column {
            id: mycol
            anchors.fill: parent

            TextField {
                id: locationName
                width: Dims.w(80)
                //% "Location Name"
                previewText: qsTrId("id-location-name-field")
                enterKeyAction: EnterKeyAction.Next
            }

            TextField {
                id: latfield
                width: Dims.w(80)
                //% "Location Latitude"
                previewText: qsTrId("id-latitude-field")
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                enterKeyAction: EnterKeyAction.Next
            }

            TextField {
                id: lonfield
                width: Dims.w(80)
                //% "Location Longitude"
                previewText: qsTrId("id-longitude-field")
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                enterKeyAction: EnterKeyAction.Next
            }
            IconButton {
                id: acceptButton
                anchors.horizontalCenter: parent.horizontalCenter
                iconName: "ios-add-circle-outline"
                onClicked: activated(placename, latfield.text, lonfield.text)
            }
        }
    }
}
