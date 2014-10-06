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

#include <algorithm>
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
#include "vatsimdata/client/controller.h"
#include "vatsimdata/client/pilot.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/fir.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsimdata/models/flighttablemodel.h"
#include "vatsinatorapplication.h"

#include "mapscene.h"

MapScene::MapScene(QObject* parent) :
    QObject(parent),
    __renderer(qobject_cast<MapRenderer*>(parent)),
    __trackedFlight(nullptr),
    __animation(nullptr) {
  Q_ASSERT(__renderer);
  
  connect(vApp()->vatsimDataHandler(),  SIGNAL(vatsimDataUpdated()),
          this,                         SLOT(__updateItems()));
  
  if (vApp()->vatsimDataHandler()->isInitialized())
    __setupItems();
  else
    connect(vApp()->vatsimDataHandler(),  SIGNAL(initialized()),
            this,                         SLOT(__setupItems()));
  connect(vApp()->settingsManager(),    SIGNAL(settingsChanged()),
          this,                         SLOT(__updateSettings()));
  __updateSettings();
}

MapScene::~MapScene() {}

void
MapScene::trackFlight(const Pilot* pilot) {
  __trackedFlight = pilot;
  emit flightTracked(__trackedFlight);
}

void
MapScene::cancelFlightTracking() {
  __trackedFlight = nullptr;
  emit flightTracked(__trackedFlight);
}

FirItem*
MapScene::findItemForFir(const Fir* fir) {
  for (FirItem* f: firItems())
    if (f->data() == fir)
      return f;
  
  return nullptr;
}

QList<const MapItem*>
MapScene::items(const QRectF& rect) const {
  QList<const MapItem*> result;
  
  for (auto it = spatial::region_cbegin(__items, rect.bottomLeft(), rect.topRight());
       it != spatial::region_cend(__items, rect.bottomLeft(), rect.topRight()); ++it) {
    if (it->second->isVisible())
      result << it->second;
  }
  
  /* Handle cross-IDL queries */
  if (rect.right() > 180.0) {
    QRectF more(QPointF(-180.0, rect.top()), QSizeF(rect.right() - 180.0, rect.height()));
    for (auto it = spatial::region_cbegin(__items, more.bottomLeft(), more.topRight());
         it != spatial::region_cend(__items, more.bottomLeft(), more.topRight()); ++it) {
      if (it->second->isVisible())
        result << it->second;
    }
  }
  
  if (rect.left() < -180.0) {
    QRectF more(QPointF(rect.left() + 360.0, rect.top()), QPointF(180.0, rect.bottom()));
    for (auto it = spatial::region_cbegin(__items, more.bottomLeft(), more.topRight());
         it != spatial::region_cend(__items, more.bottomLeft(), more.topRight()); ++it) {
      if (it->second->isVisible())
        result << it->second;
    }
  }
  
  return qMove(result);
}

void
MapScene::forEachItem(const QRectF& rect, std::function<void(const MapItem*)> function) const {
  for (auto it = spatial::region_cbegin(__items, rect.bottomLeft(), rect.topRight());
       it != spatial::region_cend(__items, rect.bottomLeft(), rect.topRight()); ++it) {
    if (it->second->isVisible())
      function(it->second);
  }
  
  /* Handle cross-IDL queries */
  if (rect.right() > 180.0) {
    QRectF more(QPointF(-180.0, rect.top()), QSizeF(rect.right() - 180.0, rect.height()));
    for (auto it = spatial::region_cbegin(__items, more.bottomLeft(), more.topRight());
         it != spatial::region_cend(__items, more.bottomLeft(), more.topRight()); ++it) {
      if (it->second->isVisible())
        function(it->second);
    }
  }
  
  if (rect.left() < -180.0) {
    QRectF more(QPointF(rect.left() + 360.0, rect.top()), QPointF(180.0, rect.bottom()));
    for (auto it = spatial::region_cbegin(__items, more.bottomLeft(), more.topRight());
         it != spatial::region_cend(__items, more.bottomLeft(), more.topRight()); ++it) {
      if (it->second->isVisible())
        function(it->second);
    }
  }
}

const MapItem*
MapScene::nearest(const LonLat& point) {
  /*
   * Dunno why, but neighbor_iterator doesn't work with const and operator++()
   * and thus we cannot make this method const :(
   */
  auto it = spatial::neighbor_begin(__items, point);
  while (!it->second->isVisible()) {
    ++it;
    Q_ASSERT(it != __items.end());
  }
  
  return it->second;
}

QList<const MapItem*>
MapScene::nearest(const LonLat& point, int max) {
  QList<const MapItem*> result;
  auto it = spatial::neighbor_begin(__items, point);
  int c = 0;
  
  while (c < max) {
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
MapScene::moveTo(const LonLat& target) {
  abortAnimation();
  
  QPropertyAnimation* animation = new QPropertyAnimation(__renderer, "center");
  animation->setDuration(500);
  animation->setEndValue(QVariant::fromValue<LonLat>(target));
  animation->setEasingCurve(QEasingCurve(QEasingCurve::InOutQuad));
  animation->start();
  
  __animation = animation;
  connect(animation, &QPropertyAnimation::finished, [this]() {
    __animation->deleteLater();
    __animation = nullptr;
  });
}

void
MapScene::zoomTo(qreal zoom) {
  abortAnimation();
  
  QPropertyAnimation* animation = new QPropertyAnimation(__renderer, "zoom");
  animation->setDuration(200);
  animation->setEndValue(zoom);
  animation->setEasingCurve(QEasingCurve(QEasingCurve::OutQuad));
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
MapScene::__addFlightItem(const Pilot* pilot) {
  /* TODO check why it can be null */
  if (pilot->position().isNull()) {
    qWarning("MapScene: %s position is null; o=%s, d=%s",
             qPrintable(pilot->callsign()), qPrintable(pilot->route().origin), qPrintable(pilot->route().destination));
    return;
  }
  
  connect(pilot, &Pilot::invalid, this, &MapScene::__removeFlightItem);
  connect(pilot, &Pilot::updated, this, &MapScene::__updateFlightItem);
  
  qDebug("MapScene: new flight item for %s at position (%f, %f)",
         qPrintable(pilot->callsign()), pilot->position().latitude(), pilot->position().longitude());
  
  FlightItem* item = new FlightItem(pilot, this);
  Q_ASSERT(item->position() == pilot->position());
  __items.insert(std::make_pair(item->position(), item));
}

void
MapScene::__setupItems() {
  for (const Airport* a: vApp()->vatsimDataHandler()->airports()) {
    AirportItem* item = new AirportItem(a, this);
    Q_ASSERT(item->position() == a->position());
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
  
  qDebug("MapScene: removing %s from the map; position: (%f, %f)",
         qPrintable(p->callsign()), p->position().latitude(), p->position().longitude());
  
  const FlightItem* citem = nullptr;
  auto it = std::find_if(spatial::equal_begin(__items, p->position()),
                         spatial::equal_end(__items, p->position()),
                         [&citem, p](const std::pair<const LonLat, const MapItem*>& it) {
    citem = dynamic_cast<const FlightItem*>(it.second);
    return citem && citem->data() == p;
  });
  Q_ASSERT(citem);
  Q_ASSERT(citem->data() == p);
  FlightItem* item = const_cast<FlightItem*>(citem);
  Q_ASSERT(item);
  
  item->deleteLater();
  __items.erase(it);
}

void
MapScene::__updateFlightItem() {
  Q_ASSERT(sender());
  
  /*
   * As there is no rebalance() method, we need to remove the corresponding item
   * and insert it back again, with the updated position.
   */
  Pilot* p = dynamic_cast<Pilot*>(sender());
  Q_ASSERT(p);
  if (p->position() == p->oldPosition()) // position didn't change
    return;
  
  auto it = std::find_if(spatial::equal_begin(__items, p->oldPosition()),
                         spatial::equal_end(__items, p->oldPosition()),
                         [p](const std::pair<const LonLat, const MapItem*>& it) {
    const FlightItem* citem = dynamic_cast<const FlightItem*>(it.second);
    return citem && citem->data() == p;
  });
  const MapItem* item = it->second;
  Q_ASSERT(item);
  __items.erase(it);
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
  
  __settings.view.airports_layer = SM::get("view.airports_layer").toBool();
  __settings.view.airport_labels = SM::get("view.airport_labels").toBool();
  __settings.view.pilots_layer = SM::get("view.pilots_layer").toBool();
  __settings.view.staffed_firs = SM::get("view.staffed_firs").toBool();
  __settings.view.unstaffed_firs = SM::get("view.unstaffed_firs").toBool();
  __settings.view.empty_airports = SM::get("view.empty_airports").toBool();
  __settings.view.pilot_labels.always = SM::get("view.pilot_labels.always").toBool();
  __settings.view.pilot_labels.airport_related = SM::get("view.pilot_labels.airport_related").toBool();
  __settings.view.pilot_labels.when_hovered = SM::get("view.pilot_labels.when_hovered").toBool();
}
