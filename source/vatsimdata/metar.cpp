/*
    metar.cpp
    Copyright (C) 2012  Michał Garapich michal@garapich.pl

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "vatsimdata/vatsimdatahandler.h"

#include "metar.h"

Metar::Metar() : __icao("ZZZZ") {}

Metar::Metar(const QString& icao, const QString& metar) :
    __icao(icao.toUpper()),
    __metar(metar),
    __lastFetchTime(QDateTime::currentDateTimeUtc()) {}

void
Metar::setMetar(const QString& metar) {
  if (metar.left(4) != icao()) {
    qWarning("Metar (%s) is not of ICAO %s!", qPrintable(metar), qPrintable(__icao));
    return;
  }
  
  __metar = metar;
  __lastFetchTime = QDateTime::currentDateTimeUtc();
}
