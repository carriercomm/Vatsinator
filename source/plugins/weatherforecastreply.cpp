/*
 * weatherforecastreply.cpp
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

#include "plugins/weatherdata.h"
#include "plugins/weatherforecastrequest.h"

#include "weatherforecastreply.h"

WeatherForecastReply::WeatherForecastReply(WeatherForecastRequest* request,
                                           QObject* parent) :
      QObject(parent),
      __request(request),
      __finished(false),
      __error(NoError) {}

WeatherForecastReply::~WeatherForecastReply() {
  delete __request;
}

void
WeatherForecastReply::appendWeatherData(WeatherData* data) {
  data->setParent(this);
  __data << data;
}

void
WeatherForecastReply::setError(WeatherForecastReply::ForecastError error) {
  __error = error;
}

void
WeatherForecastReply::setFinished(bool finished) {
  if (finished && !isFinished()) {
    __finished = finished;
    emit this->finished();
  } else {
    __finished = finished;
  }
}
