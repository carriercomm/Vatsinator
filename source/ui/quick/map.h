/*
 * map.h
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

#ifndef MAP_H
#define MAP_H

#include <QQuickItem>

class MapRenderer;
class QQuickWindow;

/**
 * The Map class is an exposed to QML map surface. It provides useful methods
 * for the qml file, such as zoom and position handling.
 */
class Map : public QQuickItem {
  Q_OBJECT

public:
  Map();
  virtual ~Map();
  
  Q_INVOKABLE void updateZoom(qreal);
  Q_INVOKABLE void updatePosition(int, int);
  
public slots:
  void sync();
  void cleanup();
  
private slots:
  void handleWindowChanged(QQuickWindow*);
  
private:
  MapRenderer* __renderer;
  
};

#endif // MAP_H
