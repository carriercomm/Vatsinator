/*
 * mapwidget.cpp
 * Copyright (C) 2013-2015  Michał Garapich <michal@garapich.pl>
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

#include <QtGlobal>
#include <QtWidgets>
#include <QtOpenGL>

#include "db/airportdatabase.h"
#include "events/mouselonlatevent.h"
#include "storage/settingsmanager.h"
#include "ui/actions/actionmenuseparator.h"
#include "ui/actions/airportdetailsaction.h"
#include "ui/actions/clientdetailsaction.h"
#include "ui/actions/firdetailsaction.h"
#include "ui/actions/metaraction.h"
#include "ui/actions/trackaction.h"
#include "ui/map/airportitem.h"
#include "ui/map/firitem.h"
#include "ui/map/flightitem.h"
#include "ui/map/mapconfig.h"
#include "ui/map/mapitem.h"
#include "ui/map/maprenderer.h"
#include "ui/map/mapscene.h"
#include "ui/models/atctablemodel.h"
#include "ui/models/flighttablemodel.h"
#include "ui/windows/vatsinatorwindow.h"
#include "ui/widgetsuserinterface.h"
#include "vatsimdata/pilot.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"
#include "config.h"

#include "mapwidget.h"

MapWidget::MapWidget(QWidget* parent) :
    QGLWidget(MapConfig::glFormat(), parent) {
  
  setAttribute(Qt::WA_NoSystemBackground);
  setAttribute(Qt::WA_OpaquePaintEvent);
  setAttribute(Qt::WA_AcceptTouchEvents);
  setAttribute(Qt::WA_TouchPadAcceptSingleTouchEvents);
  
  connect(vApp()->vatsimDataHandler(),  SIGNAL(vatsimDataUpdated()),
          this,                         SLOT(update()));
  
  connect(this, SIGNAL(menuRequest(const MapItem*)), SLOT(__showMenu(const MapItem*)));
  connect(this, SIGNAL(windowRequest(const MapItem*)),  SLOT(__showWindow(const MapItem*)));
  
  grabGesture(Qt::PinchGesture);
  
  setAutoBufferSwap(true);
}

MapWidget::~MapWidget() {
  delete __renderer;
}

bool
MapWidget::event(QEvent* event) {
  switch (event->type()) {
    case QEvent::ToolTip: {
      QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
      const MapItem* item = __underPoint(helpEvent->pos());
      if (item) {
        QToolTip::showText(helpEvent->globalPos(), item->tooltipText());
      } else {
        QToolTip::hideText();
        event->ignore();
      }
      
      return true;
    }
    
    case QEvent::Gesture:
      return gestureEvent(static_cast<QGestureEvent*>(event));
    
    default:
      return QGLWidget::event(event);
  }
}

void
MapWidget::initializeGL() {
  if (!MapRenderer::supportsRequiredOpenGLFeatures()) {
    notifyError(tr("Your system does not support required OpenGL extensions. \
                                      Please upgrade your graphic card driver."));
  }

  __renderer = new MapRenderer();
  connect(__renderer,   SIGNAL(updated()),
          this,         SLOT(update()), Qt::DirectConnection);
}

void
MapWidget::paintGL() {
  __renderer->paint();
  
  const MapItem* item = __underMouse();
  if (item) {
    if (cursor().shape() != Qt::SizeAllCursor)
      setCursor(QCursor(Qt::PointingHandCursor));
    
    __renderer->drawLines(item);
  } else {
    if (cursor().shape() != Qt::SizeAllCursor)
      setCursor(QCursor(Qt::ArrowCursor));
  }
}

void
MapWidget::resizeGL(int width, int height) {
  __renderer->setViewport(QSize(width, height)); 
}

bool
MapWidget::gestureEvent(QGestureEvent* event) {
  if (QGesture* pinch = event->gesture(Qt::PinchGesture)) {
    pinchTriggered(static_cast<QPinchGesture*>(pinch));
    event->accept(pinch);
  }
  
  return true;
}

void
MapWidget::pinchTriggered(QPinchGesture* gesture) {
  QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
  if (changeFlags & QPinchGesture::ScaleFactorChanged) {
    qreal value = gesture->scaleFactor() - gesture->lastScaleFactor();
    __renderer->setZoom(qBound(static_cast<qreal>(MapConfig::zoomMinimum()),
                               __renderer->zoom() + (__renderer->zoom() * value),
                               static_cast<qreal>(MapConfig::zoomMaximum())));
  }
}

void
MapWidget::wheelEvent(QWheelEvent* event) {
  if (event->phase() == Qt::ScrollUpdate)
    __updateZoom(event->delta() / 120);
  
  event->accept();
}

void
MapWidget::mousePressEvent(QMouseEvent* event) {
  __mousePosition.update(event->pos());
  __mousePosition.setDown(true);
  QToolTip::hideText();
  
  if (event->buttons() & Qt::LeftButton) {
    __lastClickPosition = event->pos();
  } else if (event->buttons() & Qt::RightButton) {
    const MapItem* item = __underMouse();
    emit menuRequest(item);
  }
  
  event->accept();
}

void
MapWidget::mouseReleaseEvent(QMouseEvent* event) {
  __mousePosition.update(event->pos());
  __mousePosition.setDown(false);
  setCursor(QCursor(Qt::ArrowCursor));
  
  const MapItem* item = __underMouse();
  if (item) {
    if (__lastClickPosition == __mousePosition.screenPosition()) {
      QToolTip::hideText();
      emit windowRequest(item);
    }
  } else {
      if (__renderer->scene()->trackedFlight()) {
          __renderer->scene()->moveTo(__renderer->scene()->trackedFlight()->position());
      }
  }
  
  event->accept();
}

void
MapWidget::mouseMoveEvent(QMouseEvent* event) {
  if (event->buttons() & Qt::LeftButton) {
    setCursor(QCursor(Qt::SizeAllCursor));
    
    QPoint diff = event->pos() - __mousePosition.screenPosition();
    diff.rx() *= -1;
    LonLat center = __renderer->center();
    center += __renderer->scaleToLonLat(diff);
    
    center.ry() = qBound(-90.0, center.y(), 90.0);
    if (center.x() < -180.0)
      center.rx() += 360.0;
    if (center.x() > 180.0)
      center.rx() -= 360.0;
    
    __renderer->scene()->abortAnimation();
    __renderer->setCenter(center);
  }
  __mousePosition.update(event->pos());
  update();
  event->accept();
}

void
MapWidget::keyPressEvent(QKeyEvent* event) {
  switch (event->key()) {
    case Qt::Key_PageUp:
    __updateZoom(1);
    break;
    
  case Qt::Key_PageDown:
    __updateZoom(-1);
    break;
  }
}

const MapItem*
MapWidget::__underMouse() {
  const MapItem* closest = __renderer->scene()->nearest(__mousePosition.geoPosition());
   
  if (!closest || __mousePosition.screenDistance(__renderer->mapFromLonLat(closest->position())) > MapConfig::mouseOnObject())
    return nullptr;
  else
    return closest;
}

const MapItem*
MapWidget::__underPoint(const QPoint& point) {
  const MapItem* closest = __renderer->scene()->nearest(__renderer->mapToLonLat(point).bound());
  Q_ASSERT(closest);
   
  if (__mousePosition.screenDistance(__renderer->mapFromLonLat(closest->position())) > MapConfig::mouseOnObject())
    return nullptr;
  else
    return closest;
}

void
MapWidget::__updateZoom(int steps) {
//   __renderer->updateZoom(_steps);
  __renderer->scene()->zoomTo(__renderer->zoomStep(static_cast<qreal>(steps)));
}

QMenu*
MapWidget::__itemMenu(const AirportItem* item) {
  QMenu* menu = new QMenu(item->data()->data()->icao, this);
  
  AirportDetailsAction* showAp = new AirportDetailsAction(item->data(), tr("Airport details"), this);
  connect(showAp,                       SIGNAL(triggered(const Airport*)),
          vApp()->userInterface(),      SLOT(showDetails(const Airport*)));
  menu->addAction(showAp);
  
  MetarAction* showMetar = new MetarAction(item->data()->data()->icao, this);
  connect(showMetar,                                    SIGNAL(triggered(QString)),
          vApp()->userInterface(),                      SLOT(showMetar(QString)));
  menu->addAction(showMetar);
  
  if (!item->data()->isEmpty()) {
    if (!item->data()->staff()->staff().isEmpty()) {
      menu->addSeparator();
      menu->addAction(new ActionMenuSeparator(tr("Controllers"), this));
      
      for (const Controller* c: item->data()->staff()->staff()) {
        ClientDetailsAction* cda = new ClientDetailsAction(c, c->callsign(), this);
        connect(cda, SIGNAL(triggered(const Client*)),
                vApp()->userInterface(), SLOT(showDetails(const Client*)));
        menu->addAction(cda);
      }
    }
    
    if (item->data()->countArrivals() > 0) {
      menu->addSeparator();
      menu->addAction(new ActionMenuSeparator(tr("Arrivals"), this));
      
      for (const Pilot* p: item->data()->inbounds()->flights()) {
        if (p->phase() == Pilot::Arrived) {
          ClientDetailsAction* cda = new ClientDetailsAction(p, p->callsign(), this);
          connect(cda, SIGNAL(triggered(const Client*)),
                  vApp()->userInterface(), SLOT(showDetails(const Client*)));
          menu->addAction(cda);
        }
      }
    }
    
    if (item->data()->countDepartures(false) > 0) {
      menu->addSeparator();
      menu->addAction(new ActionMenuSeparator(tr("Departures"), this));
      
      for (const Pilot* p: item->data()->outbounds()->flights()) {
        if (!p->isPrefiledOnly() && p->phase() == Pilot::Departing) {
          ClientDetailsAction* cda = new ClientDetailsAction(p, p->callsign(), this);
          connect(cda, SIGNAL(triggered(const Client*)),
                  vApp()->userInterface(), SLOT(showDetails(const Client*)));
          menu->addAction(cda);
        }
      }
    }
  }
  return menu;
}

QMenu*
MapWidget::__itemMenu(const FirItem* item) {
  QMenu* menu = new QMenu(item->data()->icao(), this);
  
  FirDetailsAction* showFir = new FirDetailsAction(
      item->data(),
      tr("%1 details").arg(item->data()->icao()),
      this
    );
  connect(showFir,                      SIGNAL(triggered(const Fir*)),
          vApp()->userInterface(),      SLOT(showDetails(const Fir*)));
  menu->addAction(showFir);
  
  for (const Controller* c: item->data()->staff()->staff()) {
    ClientDetailsAction* cda = new ClientDetailsAction(c, c->callsign(), this);
    connect(cda,                        SIGNAL(triggered(const Client*)),
            vApp()->userInterface(),    SLOT(showDetails(const Client*)));
    menu->addAction(cda);
  }
  
  for (const Controller* c: item->data()->uirStaff()->staff()) {
    ClientDetailsAction* cda = new ClientDetailsAction(c, c->callsign(), this);
    connect(cda,                        SIGNAL(triggered(const Client*)),
            vApp()->userInterface(),    SLOT(showDetails(const Client*)));
    menu->addAction(cda);
  }
  
  return menu;
}

QMenu*
MapWidget::__itemMenu(const FlightItem* item) {
   QMenu* menu = new QMenu(item->data()->callsign(), this);
  
  ClientDetailsAction* showDetails = new ClientDetailsAction(item->data(), tr("Flight details"), this);
  connect(showDetails,                  SIGNAL(triggered(const Client*)),
          vApp()->userInterface(),      SLOT(showDetails(const Client*)));
  menu->addAction(showDetails);
  
  TrackAction* trackFlight = new TrackAction(item->data(), this);
  menu->addAction(trackFlight);
  
  menu->addSeparator();

  if (!item->data()->route().origin.isEmpty()) {
    MetarAction* ma = new MetarAction(item->data()->route().origin, this);
    connect(ma,                                         SIGNAL(triggered(QString)),
            vApp()->userInterface(),                    SLOT(showMetar(QString)));
    menu->addAction(ma);
  }
  
  if (!item->data()->route().destination.isEmpty()) {
    MetarAction* ma = new MetarAction(item->data()->route().destination, this);
    connect(ma,                                         SIGNAL(triggered(QString)),
            vApp()->userInterface(),                    SLOT(showMetar(QString)));
    menu->addAction(ma);
  }
  
  return menu;
}

QMenu*
MapWidget::__itemMenu() {
  // group fetched items
  QList<const FlightItem*> flights;
  QList<const AirportItem*> airports;
  
  __renderer->scene()->nearTo(__mousePosition.geoPosition(), 10, [&](const MapItem* item) {
    if (const FlightItem* f = qobject_cast<const FlightItem*>(item)) {
      if (!f->data()->isPrefiledOnly())
        flights << f;
    } else if (const AirportItem* a = qobject_cast<const AirportItem*>(item)) {
      airports << a;
    }
  });
  
  QMenu* menu = new QMenu(tr("Nearby"), this);
  
  if (flights.size() > 0) {
    menu->addAction(new ActionMenuSeparator(tr("Flights"), this));
    
    for (const FlightItem* f: flights) {
      ClientDetailsAction* action = new ClientDetailsAction(f->data(), f->data()->callsign(), this);
      connect(action,                   SIGNAL(triggered(const Client*)),
              vApp()->userInterface(),  SLOT(showDetails(const Client*)));
      menu->addAction(action);
    }
  }
  
  if (flights.size() > 0 && airports.size() >0)
    menu->addSeparator();
  
  if (airports.size() > 0) {
    menu->addAction(new ActionMenuSeparator(tr("Airports"), this));
    
    for (const AirportItem* a: airports) {
      AirportDetailsAction* action = new AirportDetailsAction(a->data(), a->data()->icao(), this);
      connect(action,                   SIGNAL(triggered(const Airport*)),
              vApp()->userInterface(),  SLOT(showDetails(const Airport*)));
      menu->addAction(action);
    }
  }
  
  return menu;
}

void
MapWidget::__showMenu(const MapItem* item) {
  QMenu* menu;
  
  /* FIXME I don't like that part below */
  if (!item)
    menu = __itemMenu();
  else if (const AirportItem* i = qobject_cast<const AirportItem*>(item))
    menu = __itemMenu(i);
  else if (const FirItem* i = qobject_cast<const FirItem*>(item))
    menu = __itemMenu(i);
  else if (const FlightItem* i = qobject_cast<const FlightItem*>(item))
    menu = __itemMenu(i);
  else
    Q_UNREACHABLE();
  
  menu->exec(mapToGlobal(__mousePosition.screenPosition()));
  delete menu;
}

void
MapWidget::__showWindow(const MapItem* item) {
  item->showDetails();
}

MapWidget::MousePosition::MousePosition() : __down(false) {}

void
MapWidget::MousePosition::update(const QPoint& pos) {
  __screenPosition = pos;
  __geoPosition = wui()->mainWindow()->mapWidget()->renderer()->mapToLonLat(pos).bound();
  
  MouseLonLatEvent e(__geoPosition);
  qApp->notify(wui()->mainWindow(), &e);
}

qreal
MapWidget::MousePosition::screenDistance(const QPoint& point) {
  return qSqrt(
    qPow(point.x() - __screenPosition.x(), 2) +
    qPow(point.y() - __screenPosition.y(), 2)
  );
}

qreal
MapWidget::MousePosition::geoDistance(const LonLat& point) {
  return qSqrt(
    qPow(point.x() - __geoPosition.x(), 2) +
    qPow(point.y() - __geoPosition.y(), 2)
  );
}

void
MapWidget::MousePosition::setDown(bool down) {
  __down = down;
}
