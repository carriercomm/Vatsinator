/*
 * quickuserinterface.cpp
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

#include <QQuickView>

#include "events/decisionevent.h"
#include "ui/quick/map.h"

#include "quickuserinterface.h"

QuickUserInterface::QuickUserInterface(QObject* _parent) :
    UserInterface(_parent) {}

QuickUserInterface::~QuickUserInterface() {

}

void
QuickUserInterface::initialize() {
  qmlRegisterType<Map>("org.eu.vatsinator.ui", 1, 0, "Map");
  
  __engine.load(QUrl("qrc:///qmls/main.qml"));
  
  emit initialized();
}

void
QuickUserInterface::fatal(const QString&) {

}

void
QuickUserInterface::warning(const QString&) {

}

void
QuickUserInterface::statusError() {

}

void
QuickUserInterface::dataError() {

}

void
QuickUserInterface::showVatsimMessage(const QString& _msg) {

}

void
QuickUserInterface::showDetails(const Airport*) {

}

void
QuickUserInterface::showDetails(const Client*) {

}

void
QuickUserInterface::showDetails(const Fir*) {

}

void
QuickUserInterface::showMetar(const QString&) {

}

void
QuickUserInterface::showStatsDialog() {
  // enable stats anyway temporarily
  DecisionEvent* e = new DecisionEvent("statistics", DecisionEvent::Accepted);
  QCoreApplication::postEvent(vApp(), e);
}

void
QuickUserInterface::ensureMainWindowIsActive() {

}
