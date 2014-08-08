/*
 * vatsinator.qml
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

import QtQuick 2.2
import QtQuick.Controls 1.2

ApplicationWindow {
  id: vatsinatorWindow
  
  title: "Vatsinator"
  width: 640
  height: 480
  visible: true
  
  menuBar: MenuBar {
    Menu {
      title: "Vatsinator"
      
      MenuItem {
        text: qsTr("Flights")
      }
      
      MenuItem {
        text: qsTr("ATC")
      }
      
      MenuSeparator { }
      
      MenuItem {
        text: qsTr("About")
      }
    }
  }
  
  Text {
    text: qsTr("Map will be here")
    anchors.centerIn: parent
  }
}
