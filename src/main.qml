/*
 * Copyright (C) 2028 - Ed Beroset <beroset@ieee.org>
 *               2018 - Florent Revest <revestflo@gmail.com>
 *               2016 - Andrew Branson <andrew.branson@jollamobile.com>
 *                      Ruslan N. Marchenko <me@ruff.mobi>
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
import Nemo.Configuration 1.0
import Qt.labs.settings 1.0

Application {
    id: weatherfetch
    centerColor: "#b04d1c"
    outerColor: "#421c0a"

    property var savedlocations: ""
    property int locationPrecision: 4

    ConfigurationValue {
        id: cityName

        key: "/org/asteroidos/weather/city-name"
        //% "Unknown"
        defaultValue: qsTrId("id-unknown")
    }

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
                        var zoomlat = locations.count ? parseFloat(locations.get(0).lat): 51.5
                        var zoomlong = locations.count ? parseFloat(locations.get(0).lng): -0.144
                        console.log("locations count = ", locations.count)
                        console.log("zoomlat = ", zoomlat)
                        console.log("zoomlong = ", zoomlong)
                        console.log("resolved url = ", Qt.resolvedUrl("LocationPicker.qml"))
                        var newloc = layerStack.push(Qt.resolvedUrl("LocationPicker.qml"), {lat:zoomlat, lon:zoomlong});
                        newloc.activated.connect(selected)
                        function selected(name, lat, lng) {
                            newloc.activated.disconnect(selected);
                            console.log(name, lat, lng);
                            locations.append({"name":name,
                                    "lat":lat.toFixed(locationPrecision).toString(),
                                    "lng":lng.toFixed(locationPrecision).toString()} );
                        }
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

            Row {
                width: parent.width
                height: Dims.l(20)
                Label {
                    //% "Fetch weather data"
                    text: qsTrId("id-weatherfetch-fetch")
                    font.pixelSize: Dims.l(6)
                    verticalAlignment: Text.AlignVCenter
                    width: parent.width * 0.7143
                    wrapMode: Text.Wrap
                }
                IconButton {
                    id: fetchButton
                    iconName: "ios-arrow-dropright"
                    height: parent.height
                    width: height
                    onClicked: {
                        cityName.value = locations.get(0).name;
                        console.log("getting weather for ", locations.get(0).name, "( ", locations.get(0).lat, ", ", locations.get(0).lng, " )");
                        getDummyWeatherForecast(locations.get(0).lat, locations.get(0).lng, settings.apikey)
                    }
                }
            }

            Item { width: parent.width; height: Dims.l(10) }
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

    Component.onDestruction: {
        var datamodel = []
        for (var i = 0; i < locations.count; ++i) {
            datamodel.push(locations.get(i))
        }
        savedlocations = JSON.stringify(datamodel)
    }

    function getDummyWeatherForecast(lat, lon, apikey) {
        const var dummydata='{"lat":35.8586,"lon":-79.1032,"timezone":"America/New_York","timezone_offset":-14400,"daily":[{"dt":1664038800,"sunrise":1664017551,"sunset":1664061060,"moonrise":1664012340,"moonset":1664060040,"moon_phase":0.96,"temp":{"day":296.93,"min":282.52,"max":297.31,"night":289.41,"eve":295.02,"morn":282.52},"feels_like":{"day":296.15,"night":288.27,"eve":294.31,"morn":282.52},"pressure":1019,"humidity":30,"dew_point":278.34,"wind_speed":3.47,"wind_deg":229,"wind_gust":9.92,"weather":[{"id":801,"main":"Clouds","description":"few clouds","icon":"02d"}],"clouds":20,"pop":0,"uvi":6.95},{"dt":1664125200,"sunrise":1664103997,"sunset":1664147371,"moonrise":1664102400,"moonset":1664148000,"moon_phase":0,"temp":{"day":298.83,"min":286.99,"max":301.59,"night":291.48,"eve":297.27,"morn":287.33},"feels_like":{"day":298.47,"night":291.77,"eve":297.17,"morn":286.42},"pressure":1011,"humidity":39,"dew_point":283.87,"wind_speed":6.02,"wind_deg":215,"wind_gust":12.38,"weather":[{"id":501,"main":"Rain","description":"moderate rain","icon":"10d"}],"clouds":99,"pop":0.73,"rain":2.03,"uvi":5.33},{"dt":1664211600,"sunrise":1664190443,"sunset":1664233682,"moonrise":1664192580,"moonset":1664235960,"moon_phase":0.02,"temp":{"day":299.9,"min":286.46,"max":300.97,"night":289.32,"eve":293.61,"morn":286.46},"feels_like":{"day":299.21,"night":288.25,"eve":292.81,"morn":286.22},"pressure":1011,"humidity":24,"dew_point":277.79,"wind_speed":4.66,"wind_deg":285,"wind_gust":10.31,"weather":[{"id":800,"main":"Clear","description":"clear sky","icon":"01d"}],"clouds":0,"pop":0.45,"uvi":6.32},{"dt":1664298000,"sunrise":1664276889,"sunset":1664319993,"moonrise":1664282760,"moonset":1664323980,"moon_phase":0.06,"temp":{"day":297.4,"min":284.42,"max":297.56,"night":286.34,"eve":289.55,"morn":284.42},"feels_like":{"day":296.38,"night":284.89,"eve":288.16,"morn":283.12},"pressure":1016,"humidity":19,"dew_point":272.76,"wind_speed":4.69,"wind_deg":312,"wind_gust":8.27,"weather":[{"id":800,"main":"Clear","description":"clear sky","icon":"01d"}],"clouds":0,"pop":0,"uvi":6.53},{"dt":1664384400,"sunrise":1664363336,"sunset":1664406305,"moonrise":1664373180,"moonset":1664412240,"moon_phase":0.09,"temp":{"day":294.03,"min":283.99,"max":294.03,"night":286.55,"eve":287.89,"morn":283.99},"feels_like":{"day":292.75,"night":285.12,"eve":286.41,"morn":282.57},"pressure":1022,"humidity":22,"dew_point":271.43,"wind_speed":4.26,"wind_deg":1,"wind_gust":7.73,"weather":[{"id":804,"main":"Clouds","description":"overcast clouds","icon":"04d"}],"clouds":98,"pop":0,"uvi":5.62},{"dt":1664470800,"sunrise":1664449783,"sunset":1664492616,"moonrise":1664463720,"moonset":1664500800,"moon_phase":0.13,"temp":{"day":290.26,"min":285.81,"max":290.31,"night":287.56,"eve":288.42,"morn":285.81},"feels_like":{"day":289.33,"night":286.49,"eve":287.39,"morn":284.88},"pressure":1026,"humidity":50,"dew_point":279.78,"wind_speed":5.5,"wind_deg":45,"wind_gust":11.91,"weather":[{"id":804,"main":"Clouds","description":"overcast clouds","icon":"04d"}],"clouds":100,"pop":0.07,"uvi":0.07},{"dt":1664557200,"sunrise":1664536230,"sunset":1664578928,"moonrise":1664554320,"moonset":1664589720,"moon_phase":0.17,"temp":{"day":285.68,"min":284.88,"max":289.72,"night":289.72,"eve":288.41,"morn":284.88},"feels_like":{"day":285.52,"night":290.02,"eve":288.55,"morn":284.59},"pressure":1020,"humidity":97,"dew_point":285.38,"wind_speed":6.46,"wind_deg":49,"wind_gust":15.97,"weather":[{"id":502,"main":"Rain","description":"heavy intensity rain","icon":"10d"}],"clouds":100,"pop":1,"rain":71.4,"uvi":1},{"dt":1664643600,"sunrise":1664622677,"sunset":1664665241,"moonrise":1664644980,"moonset":1664679240,"moon_phase":0.2,"temp":{"day":293.05,"min":290.52,"max":294.03,"night":292.77,"eve":292.86,"morn":292.88},"feels_like":{"day":293.65,"night":293.37,"eve":293.47,"morn":293.49},"pressure":1012,"humidity":98,"dew_point":292.78,"wind_speed":4.01,"wind_deg":154,"wind_gust":10.19,"weather":[{"id":501,"main":"Rain","description":"moderate rain","icon":"10d"}],"clouds":100,"pop":1,"rain":13.8,"uvi":1}]}';
        updateForecast(dummydata);
    }
    function getWeatherForecast(lat, lon, apikey) {
        const xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
            if (xhttp.readyState == XMLHttpRequest.DONE && xhttp.status == 200) {
                updateForecast(xhttp.responseText.toString());
            }
        };
        xhttp.onerror = function() {
            console.log("Error fetching weather data: ", xhttp.errorText);
        }
        var omit = "current,minutely,hourly,alerts";
        var url = "https://api.openweathermap.org/data/2.5/onecall?lat="+lat+"&lon="+lon+"&exclude="+omit+"&appid="+apikey;
        console.log("url: ", url);
        xhttp.open("GET", url);
        xhttp.send();
    }

    function updateForecast(forecast) {
        console.log(forecast);
        var wx = JSON.parse(forecast);
        wx["daily"].forEach(function(day, i) {
            console.log(i, day.dt, day.weather[0].id, day.temp.min, day.temp.max);
            // TODO: parse the data and send it via D-Bus
        });
    }
}
