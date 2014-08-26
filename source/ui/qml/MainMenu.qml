/*
 * MainMenu.qml
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
import QtGraphicalEffects 1.0

Item {
  id: root
  
  width: parent.width
  height: parent.height
  x: width * -1
  
  /* Semi-transparent background */
  Rectangle {
    anchors.fill: parent
    color: "#262626"
    opacity: 0.8
  }
  
  
  /* Menu container */
  Rectangle {
    id: container
    
    anchors.fill: parent
    anchors.rightMargin: parent.width * 0.1
    color: "#262626"
    
  }
  
  /* Menu drawer */
  Item {
    id: drawerContainer
    
    height: drawer.height + (2 * drawerShadow.glowRadius)
    width: 64 + drawer.anchors.leftMargin + (2 * drawerShadow.glowRadius)
    anchors {
      bottom: parent.bottom
      bottomMargin: parent.height / 20
      left: parent.right
    }
    
    RectangularGlow {
      id: drawerShadow
      
      anchors.fill: parent
      glowRadius: 1.0
      color: "#393939"
      cornerRadius: 11
      anchors.leftMargin: drawer.anchors.leftMargin
    }
    
    Rectangle {
      id: drawer
      
      color: "white"
      border.width: 0
      radius: 10
      height: 128
      width: 64
      anchors.left: parent.left
      anchors.verticalCenter: parent.verticalCenter
      anchors.leftMargin: -10
      
      Image {
        width: 48
        height: 48
        fillMode: Image.Stretch
        source: "ic_drawer.png"
        anchors.centerIn: parent
      }
    }
    
    MouseArea {
      anchors.fill: parent
      onClicked: {
        root.state = "shown"
        drawerContainer.visible = false
      }
    }
  }
  
  MouseArea {
    anchors {
      left: container.left
      right: root.right
      bottom: root.bottom
      top: root.top
    }
    
    onClicked: {
      root.state = ""
      drawerContainer.visible = true
    }
  }
  
  states: [
    State {
      name: "shown"
      PropertyChanges { target: root; x: 0 }
    }
  ]
  
  transitions: [
    Transition {
      to: "*"
      PropertyAnimation { target: root; property: "x"; duration: 200; easing.type: Easing.OutQuad }
    }
  ]
}
