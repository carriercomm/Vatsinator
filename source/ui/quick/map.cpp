/*
 * map.cpp
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

#include <QtQuick>

#include "ui/map/maprenderer.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "map.h"

Map::Map() :
    __renderer(nullptr) {
  setFlag(QQuickItem::ItemHasContents);
  connect(this, SIGNAL(windowChanged(QQuickWindow*)), SLOT(handleWindowChanged(QQuickWindow*)));
}

Map::~Map() {

}

void
Map::updateZoom(qreal _factor) {
  if (__renderer) {
    __renderer->setZoom(__renderer->zoom() + __renderer->zoom() * _factor);
  }
  
  if (window())
      window()->update();
}

void
Map::updatePosition(int _x, int _y) {
  if (__renderer) {
    LonLat center = __renderer->center();
    QPoint p(_x, -_y);
    center -= __renderer->scaleToLonLat(p);
    center.ry() = qBound(-90.0, center.y(), 90.0);
    if (center.x() < -180.0)
      center.rx() += 360.0;
    if (center.x() > 180.0)
      center.rx() -= 360.0;
    
    __renderer->setCenter(center);
  }
  
  if (window())
      window()->update();
}

void
Map::sync() {
  if (!__renderer) {
    __renderer = new MapRenderer();
    connect(window(),   SIGNAL(beforeRendering()),
            __renderer, SLOT(paint()),  Qt::DirectConnection);
  }
  
  __renderer->setViewport(window()->size() * window()->devicePixelRatio());
}

void
Map::cleanup() {
  if (__renderer) {
    delete __renderer;
    __renderer = nullptr;
  }
}

void
Map::handleWindowChanged(QQuickWindow* _win) {
  if (_win) {
    connect(_win,       SIGNAL(beforeSynchronizing()),
            this,       SLOT(sync()), Qt::DirectConnection);
    connect(_win,       SIGNAL(sceneGraphInvalidated()),
            this,       SLOT(cleanup()), Qt::DirectConnection);
    connect(vApp()->vatsimDataHandler(), 
                        SIGNAL(vatsimDataUpdated()),
            _win,       SLOT(update()));
    _win->setClearBeforeRendering(false);
  }
}