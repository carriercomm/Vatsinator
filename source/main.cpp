/*
    main.cpp
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

#include "vatsimdata/lonlat.h"
#include "vatsinatorapplication.h"
#include "config.h"

int main(int argc, char** argv) {
  Q_INIT_RESOURCE(imgs);
  Q_INIT_RESOURCE(weather);
  
  QGuiApplication::setApplicationName("Vatsinator");
  QGuiApplication::setOrganizationName("VatsinatorTeam");
  QGuiApplication::setApplicationVersion(VATSINATOR_VERSION);
  
  qRegisterMetaType<LonLat>("LonLat");
  qRegisterMetaTypeStreamOperators<LonLat>("LonLat");
  
  return VatsinatorApplication(argc, argv).exec();
}
