/*
    uir.cpp
    Copyright (C) 2012-2014  Michał Garapich michal@garapich.pl

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

#include "vatsimdata/fir.h"
#include "vatsimdata/client/controller.h"
#include "vatsimdata/models/controllertablemodel.h"

#include "uir.h"

Uir::Uir(const QString& _icao, QObject* _parent) :
    QObject(_parent),
    __icao(_icao),
    __staff(new ControllerTableModel(this)) {}

void
Uir::addFir(Fir* _f) {
  __range.push_back(_f);
}

void
Uir::addStaff(const Controller* _c) {
  __staff->add(_c);
  connect(_c,           SIGNAL(updated()),
          this,         SIGNAL(updated()));
  connect(_c,           SIGNAL(destroyed(QObject*)),
          this,         SIGNAL(updated()), Qt::DirectConnection);
  emit updated();
  
  for (Fir* f: range())
    f->addUirStaff(_c);
}

bool
Uir::isEmpty() const {
  return __staff->staff().isEmpty();
}

