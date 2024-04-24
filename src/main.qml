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

import QtQuick 2.15
import QtQml.Models 2.15
import org.asteroid.controls 1.0
import org.asteroid.weatherfetch 1.0
import Nemo.Configuration 1.0
import Nemo.Notifications 1.0
import Qt.labs.settings 1.0

Application {
    id: weatherfetch
    centerColor: "#b04d1c"
    outerColor: "#421c0a"

    property var savedlocations: ""
    property int locationPrecision: 4
    property bool goodFetch: false
    property string newCityName: ""

    ConfigurationValue {
        id: cityName
        key: "/org/asteroidos/weather/city-name"
        //% "Unknown"
        defaultValue: qsTrId("id-unknown")
    }

    LayerStack {
        id: layerStack
        firstPage: firstPageComponent
    }

    Component  { id: locationDialog;   LocationDialog   { } }

    WeatherParser { id: weatherParser }

    Settings {
        id: settings
        category: "Weather"
        property alias savedlocations: weatherfetch.savedlocations
        property string apikey: "36cc791575eef0fc1a4560ac24475dad"
    }

    Component {
        id: dragDelegate

        MouseArea {
            id: dragArea

            property bool held: false

            anchors { left: parent.left; right: parent.right }
            height: content.height
            drag.target: held ? content : undefined
            drag.axis: Drag.YAxis

            onPressAndHold: {
                held = true
            }
            onReleased: {
                held = false
            }

            Rectangle {
                id: content
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    verticalCenter: parent.verticalCenter
                }
                width: dragArea.width
                height: databox.implicitHeight + 4

                border.width: 1
                border.color: "lightsteelblue"

                color: dragArea.held ? "lightsteelblue" : "white"
                Behavior on color { ColorAnimation { duration: 100 } }

                radius: 2
                Drag.active: dragArea.held
                Drag.source: dragArea
                Drag.hotSpot.x: width / 2
                Drag.hotSpot.y: height / 2
                states: State {
                    when: dragArea.held

                    ParentChange { target: content; parent: listroot }
                    AnchorChanges {
                        target: content
                        anchors { horizontalCenter: undefined; verticalCenter: undefined }
                    }
                }

                Column {
                    id: databox
                    anchors { fill: parent; margins: 2 }

                    Label {
                        id: loclabel
                        color: "black"
                        text: "<b>" + name + "</b>"
                    }
                    Text {
                        text: lat  + " " + lng
                    }
                }
            }
            DropArea {
                anchors { fill: parent; margins: 10 }
                onEntered: {
                    locations.move(
                                drag.source.DelegateModel.itemsIndex,
                                dragArea.DelegateModel.itemsIndex,
                                1)
                }
            }
        }
    }

    ListModel {
        id: locations
    }

    DelegateModel {
        id: visualModel
        model: locations
        delegate: dragDelegate
    }

    Component {
        id: firstPageComponent
        Item {
            PageHeader {
                //% "Fetch weather data"
                text: qsTrId("id-weatherfetch")
            }

            Flickable {
                anchors.fill: parent
                contentHeight: onOffSettings.implicitHeight
                boundsBehavior: Flickable.DragOverBounds
                flickableDirection: Flickable.VerticalFlick
                anchors.margins: Dims.l(15)

                Column {
                    id: onOffSettings
                    anchors.fill: parent

                    Rectangle {
                        id: listroot
                        width: parent.width
                        height: Dims.h(40)

                        ListView {
                            id: locList
                            anchors.fill: parent
                            anchors.margins: 2
                            model: visualModel
                            spacing: 4
                        }
                    }
                    Item { width: parent.width; height: Dims.l(14) }

                    Item {
                        width: parent.width
                        height: Dims.l(20)

                        MouseArea {
                            id: mouseArea
                            anchors.fill: parent
                            hoverEnabled: true

                            Notification {
                                id: startmessage
                                appName: "asteroid-weatherfetch"
                                //% "Attempting to fetch weather data"
                                previewBody: qsTrId("id-weatherfetch-trying")
                            }
                            Notification {
                                id: donemessage
                                appName: "asteroid-weatherfetch"
                                //% "Weather data unchanged"
                                previewBody: goodFetch ? newCityName : qsTrId("id-weatherfetch-unchanged")
                                //% "Weather fetch succeeded"
                                previewSummary: goodFetch ? qsTrId("id-weatherfetch-success") :
                                    //% "Weather fetch failed"
                                    qsTrId("id-weatherfetch-fail")
                            }
                            onClicked: {
                                newCityName = locations.get(0).name
                                startmessage.previewSummary = newCityName
                                startmessage.publish()
                                console.log("getting weather for ", newCityName, "( ", locations.get(0).lat, ", ", locations.get(0).lng, " )");
                                getWeatherForecast(newCityName, locations.get(0).lat, locations.get(0).lng, settings.apikey, donemessage)
                            }
                        }

                        Rectangle {
                            anchors.fill: parent
                            color: "white"
                            opacity: mouseArea.containsPress ? 0.2 : 0
                        }
                        Label {
                            //% "Fetch weather data"
                            text: qsTrId("id-weatherfetch-fetch")
                            font.pixelSize: Dims.l(6)
                            verticalAlignment: Text.AlignVCenter
                            width: parent.width * 0.7143
                            wrapMode: Text.Wrap
                        }
                        Icon {
                            id: fetchButton
                            name: "ios-arrow-dropright"
                            height: parent.height
                            width: height
                            anchors.right: parent.right
                        }
                    }
                    Row {
                        Label {
                            //% "Add location"
                            text: qsTrId("id-weatherfetch-addlocation")
                            font.pixelSize: parent.height * 0.3
                            verticalAlignment: Text.AlignVCenter
                            wrapMode: Text.Wrap
                            width: parent.width * 0.7143
                            height: parent.height
                        }

                        IconButton {
                            id: addButton
                            iconName: "ios-add-circle-outline"
                            onClicked: {
                                console.log("locations count = ", locations.count)
                                function addLoc(name, lat, lng) {
                                    console.log(name, lat, lng);
                                    locations.append({"name":name,
                                            "lat":lat.toFixed(locationPrecision).toString(),
                                            "lng":lng.toFixed(locationPrecision).toString()} );
                                }
                                layerStack.push(locationDialog, {selected: addLoc});
                            }
                        }
                    }
                    Row {
                        Label {
                            //% "Delete last location"
                            text: qsTrId("id-weatherfetch-dellocation")
                            font.pixelSize: parent.height * 0.3
                            verticalAlignment: Text.AlignVCenter
                            wrapMode: Text.Wrap
                            width: parent.width * 0.7143
                            height: parent.height
                        }
                        IconButton {
                            id: delButton
                            iconName: "ios-remove-circle-outline"
                            onClicked: locations.remove(locations.count - 1)
                        }
                    }

                    Item { width: parent.width; height: Dims.l(10) }
                }
            }
            Component.onDestruction: {
                saveModel()
            }
        }
    }


    Component.onCompleted: {
        if (savedlocations) {
            locations.clear()
            var datamodel = JSON.parse(savedlocations)
            for (var i=0; i < datamodel.length; ++i) {
                locations.append(datamodel[i])
            }
        }
    }

    function saveModel() {
        var datamodel = []
        for (var i = 0; i < locations.count; ++i) {
            datamodel.push(locations.get(i))
        }
        savedlocations = JSON.stringify(datamodel)
    }

    function getWeatherForecast(cityName, lat, lon, apikey, donemessage) {
        const xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
            if (xhttp.readyState === XMLHttpRequest.DONE)  {
                if (xhttp.status === 200) {
                    updateForecast(xhttp.responseText.toString());
                    goodFetch = true;
                    donemessage.publish();
                } else {
                    goodFetch = false;
                    donemessage.publish();
                }
            }
        };
        xhttp.onerror = function() {
            console.log("Error fetching weather data: ", xhttp.errorText);
        }
        var url = weatherParser.createUrl(cityName, lat, lon, apikey);
        console.log("url: ", url);
        xhttp.open("GET", url);
        xhttp.send();
    }

    function updateForecast(forecast) {
        weatherParser.updateWeather(newCityName, forecast);
    }
}
