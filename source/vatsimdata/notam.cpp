/*
 * notam.cpp
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

#include "notam.h"
#include "defines.h"

Notam::Notam(QString _ident) :
     __ident(std::move(_ident)) {}

Notam::Notam(QString _ident, QString _icao, QString _notam, QString _url,
             QDateTime _from, QDateTime _to, Notam::Type _type) :
     __ident(std::move(_ident)),
     __icao(std::move(_icao)),
     __notam(std::move(_notam)),
     __url(std::move(_url)),
     __from(std::move(_from)),
     __to(std::move(_to)),
     __type(_type) {}

void
Notam::setIcao(const QString& _icao) {
  __icao = _icao;
}

void
Notam::setNotam(const QString& _notam) {
  __notam = _notam;
}

void
Notam::setUrl(const QString& _url) {
  __url = _url;
}

void
Notam::setFrom(const QDateTime& _from) {
  __from = _from;
}

void
Notam::setTo(const QDateTime& _to) {
  __to = _to;
}

void
Notam::setType(Notam::Type _type) {
  __type = _type;
}
