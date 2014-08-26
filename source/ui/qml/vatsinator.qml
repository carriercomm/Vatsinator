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
import VatsinatorQML 1.0

ApplicationWindow {
  id: vatsinatorWindow
    
  height: 480
  title: "Vatsinator"
  width: 320
  
  Loader {
    id: loader
  }
  
  Map {
    id: map
  }
  
  PinchArea {
    anchors.fill: parent
    
    onPinchUpdated: {
      map.updateZoom(pinch.scale - pinch.previousScale)
    }
    
    MouseArea {
      anchors.fill: parent
      
      property int lastX
      property int lastY
      
      onPressed: {
        lastX = mouse.x;
        lastY = mouse.y;
        mouse.accepted = true;
      }
      
      onPositionChanged: {
        map.updatePosition(mouse.x - lastX, mouse.y - lastY);
        lastX = mouse.x;
        lastY = mouse.y;
      }
    }
  }
  
  MainMenu {
    id: mainMenu
  }
  
  Component.onCompleted: visible = true
}
