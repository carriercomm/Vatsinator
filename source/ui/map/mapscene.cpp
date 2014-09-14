/*
 * mapscene.cpp
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

#include <utility>
#include <QtCore>
#include <spatial/point_multimap.hpp>
#include <spatial/neighbor_iterator.hpp>
#include <spatial/region_iterator.hpp>

#include "db/airportdatabase.h"
#include "db/firdatabase.h"
#include "storage/settingsmanager.h"
#include "ui/map/airportitem.h"
#include "ui/map/approachcircleitem.h"
#include "ui/map/firitem.h"
#include "ui/map/flightitem.h"
#include "ui/map/maprenderer.h"
#include "ui/map/uiritem.h"
#include "ui/widgets/mapwidget.h"
#include "vatsimdata/client/controller.h"
#include "vatsimdata/client/pilot.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/fir.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsimdata/models/flighttablemodel.h"
#include "vatsinatorapplication.h"

#include "mapscene.h"

MapScene::MapScene(QObject* _parent) :
    QObject(_parent),
    __renderer(qobject_cast<MapRenderer*>(parent())),
    __trackedFlight(nullptr),
    __animation(nullptr) {
  Q_ASSERT(__renderer);
  
  connect(vApp()->vatsimDataHandler(),  SIGNAL(vatsimDataUpdated()),
          this,                         SLOT(__updateItems()));
  connect(vApp()->vatsimDataHandler(),  SIGNAL(initialized()),
          this,                         SLOT(__setupItems()));
  connect(vApp()->settingsManager(),    SIGNAL(settingsChanged()),
          this,                         SLOT(__updateSettings()));
  __updateSettings();
}

MapScene::~MapScene() {}

void
MapScene::trackFlight(const Pilot* _p) {
  __trackedFlight = _p;
  emit flightTracked(__trackedFlight);
}

void
MapScene::cancelFlightTracking() {
  __trackedFlight = nullptr;
  emit flightTracked(__trackedFlight);
}

FirItem*
MapScene::findItemForFir(const Fir* _fir) {
  for (FirItem* f: firItems())
    if (f->data() == _fir)
      return f;
  
  return nullptr;
}

QList<const MapItem*>
MapScene::items(const QRectF& _rect) const {
  QList<const MapItem*> result;
  
  for (auto it = spatial::region_cbegin(__items, _rect.bottomLeft(), _rect.topRight());
       it != spatial::region_cend(__items, _rect.bottomLeft(), _rect.topRight()); ++it) {
    if (it->second->isVisible())
      result << it->second;
  }
  
  /* Handle cross-IDL queries */
  if (_rect.right() > 180.0) {
    QRectF more(QPointF(-180.0, _rect.top()), QSizeF(_rect.right() - 180.0, _rect.height()));
    for (auto it = spatial::region_cbegin(__items, more.bottomLeft(), more.topRight());
         it != spatial::region_cend(__items, more.bottomLeft(), more.topRight()); ++it) {
      if (it->second->isVisible())
        result << it->second;
    }
  }
  
  if (_rect.left() < -180.0) {
    QRectF more(QPointF(_rect.left() + 360.0, _rect.top()), QPointF(180.0, _rect.bottom()));
    for (auto it = spatial::region_cbegin(__items, more.bottomLeft(), more.topRight());
         it != spatial::region_cend(__items, more.bottomLeft(), more.topRight()); ++it) {
      if (it->second->isVisible())
        result << it->second;
    }
  }
  
  return qMove(result);
}

const MapItem*
MapScene::nearest(const LonLat& _target) {
  /*
   * Dunno why, but neighbor_iterator doesn't work with const and operator++()
   * and thus we cannot make this method const.
   */
  auto it = spatial::neighbor_begin(__items, _target);
  while (!it->second->isVisible()) {
    ++it;
    Q_ASSERT(it != __items.end());
  }
  
  return it->second;
}

QList<const MapItem*>
MapScene::nearest(const LonLat& _target, int _n) {
  QList<const MapItem*> result;
  auto it = spatial::neighbor_begin(__items, _target);
  int c = 0;
  
  while (c < _n) {
    if (it->second->isVisible()) {
      result << it->second;
      c += 1;
    }
    ++it;
    Q_ASSERT(it != __items.end());
  }
  
  return qMove(result);
}

void
MapScene::moveTo(const LonLat& _target) {
  abortAnimation();
  
  QPropertyAnimation* animation = new QPropertyAnimation(__renderer, "center");
  animation->setDuration(500);
  animation->setEndValue(QVariant::fromValue<LonLat>(_target));
  animation->setEasingCurve(QEasingCurve(QEasingCurve::InOutQuad));
  animation->start();
  
  __animation = animation;
  connect(animation, &QPropertyAnimation::finished, [this]() {
    __animation->deleteLater();
    __animation = nullptr;
  });
}

void
MapScene::abortAnimation() {
  if (__animation) {
    __animation->stop();
    __animation->deleteLater();
    __animation = nullptr;
  }
}

void
MapScene::__addFlightItem(const Pilot* _p) {
  /* TODO check why it can be null */
  if (_p->position().isNull()) {
    qWarning("MapScene: %s position is null; o=%s, d=%s",
             qPrintable(_p->callsign()), qPrintable(_p->route().origin), qPrintable(_p->route().destination));
    return;
  }
  
  connect(_p,           SIGNAL(invalid()),
          this,         SLOT(__removeFlightItem()));
  connect(_p,           SIGNAL(updated()),
          this,         SLOT(__updateFlightItem()));
  FlightItem* item = new FlightItem(_p, this);
  Q_ASSERT(item->position() == _p->position());
  __items.insert(std::make_pair(item->position(), item));
}

void
MapScene::__setupItems() {
  for (const Airport* a: vApp()->vatsimDataHandler()->airports()) {
    AirportItem* item = new AirportItem(a, this);
    __items.insert(std::make_pair(item->position(), item));
  }
  
  for (const Fir* f: vApp()->vatsimDataHandler()->firs()) {
    if (f->data()->header.textPosition.x != 0.0 && f->data()->header.textPosition.y != 0.0) {
      FirItem* item = new FirItem(f, this);
      __firItems << item;
      __items.insert(std::make_pair(item->position(), item));
    }
  }
  
  for (const Uir* u: vApp()->vatsimDataHandler()->uirs()) {
    __uirItems << new UirItem(u, this);
  }
  
  for (auto c: vApp()->vatsimDataHandler()->clients())
    if (Pilot* p = dynamic_cast<Pilot*>(c)) {
      /* TODO handle prefiled flights */
      if (p->phase() != Pilot::Arrived && !p->isPrefiledOnly())
        __addFlightItem(p);
    }
}

void
MapScene::__updateItems() {
  for (Client* c: vApp()->vatsimDataHandler()->newClients())
    if (Pilot* p = dynamic_cast<Pilot*>(c)) {
      if (p->phase() != Pilot::Arrived && !p->isPrefiledOnly())
        __addFlightItem(p);
    }
}

void
MapScene::__removeFlightItem() {
  Q_ASSERT(sender());
  
  Pilot* p = dynamic_cast<Pilot*>(sender());
  auto it = __items.find(p->position());
  Q_ASSERT(it != __items.end());
  const FlightItem* citem = dynamic_cast<const FlightItem*>(it->second);
  Q_ASSERT(citem);
  FlightItem* item = const_cast<FlightItem*>(citem);
  Q_ASSERT(item);
  item->deleteLater();
  __items.erase(it);
}

void
MapScene::__updateFlightItem() {
  /*
   * As there is no rebalance() method, we need to remove the corresponding item
   * and insert it back again, with the updated position.
   */
  Pilot* p = dynamic_cast<Pilot*>(sender());
  if (p->position() == p->oldPosition())
    return;
  
  auto it = __items.find(p->oldPosition());
  Q_ASSERT(it != __items.end());
  
  const MapItem* item = it->second;
  __items.erase(p->oldPosition());
  __items.insert(std::make_pair(p->position(), item));
}

void
MapScene::__updateSettings() {
  __settings.misc.zoom_coefficient = SM::get("map.zoom_coefficient").toInt();
  
  __settings.colors.lands = SM::get("map.lands_color").value<QColor>();
  __settings.colors.seas = SM::get("map.seas_color").value<QColor>();
  __settings.colors.staffed_fir_borders = SM::get("map.staffed_fir_borders_color").value<QColor>();
  __settings.colors.staffed_fir_background = SM::get("map.staffed_fir_background_color").value<QColor>();
  __settings.colors.staffed_uir_borders = SM::get("map.staffed_uir_borders_color").value<QColor>();
  __settings.colors.staffed_uir_background = SM::get("map.staffed_uir_background_color").value<QColor>();
  __settings.colors.unstaffed_fir_borders = SM::get("map.unstaffed_fir_borders_color").value<QColor>();
  __settings.colors.approach_circle = SM::get("map.approach_circle_color").value<QColor>();
}
