/*
 * weatherforecastinterface.h
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

#ifndef WEATHERFORECASTINTERFACE_H
#define WEATHERFORECASTINTERFACE_H

#include <QObject>
#include <QString>
#include <QtPlugin>

#include "plugins/weatherforecastrequest.h"
#include "plugins/weatherforecastreply.h"

class WeatherForecastInterface {

public:
  
  /**
   * Begins fetching the weather forecast.
   */
  virtual WeatherForecastReply* fetch(WeatherForecastRequest* request) = 0;
  
};

Q_DECLARE_INTERFACE(WeatherForecastInterface,
                    "org.eu.vatsinator.Vatsinator.WeatherForecastInterface")

#endif // WEATHERFORECASTINTERFACE_H
