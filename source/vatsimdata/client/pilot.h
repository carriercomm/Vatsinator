/*
    pilot.h
    Copyright (C) 2012-2013  Michał Garapich michal@garapich.pl

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


#ifndef PILOT_H
#define PILOT_H

#include <QStringList>
#include <QString>

#include "vatsimdata/client.h"

struct Point;

class Pilot : public Client {

  /**
   * This class contains info about one particular pilot - his
   * flight plan, actual position, plane, etc etc.
   */

public:
  /* Types */
  enum FlightRules {
    Ifr, Vfr
  };
  
  enum Status {
    Departing,
    Airborne,
    Arrived
  };
  
  struct Pressure {
    QString ihg;
    QString mb;
  };
  
  struct Route {
    QString origin;
    QString destination;
    QString route;
    QString altitude;
  };
  
  Pilot() = delete;
  
  /**
   * @param list Data.
   * @param prefiled Indicates whether the flight is only prefiled.
   */
  Pilot(const QStringList&, bool = false);
  virtual ~Pilot();
  
  /**
   * The current altitude.
   */
  inline int altitude() const { return __altitude; }
  
  /**
   * The client's ground speed, in knots.
   */
  inline int groundSpeed() const { return __groundSpeed; }
  
  /**
   * The client's squawk code.
   * It's string, not int, as squawk might start with '0'.
   */
  inline const QString& squawk() const { return __squawk; }
  
  /**
   * The client's aircraft.
   */
  inline const QString& aircraft() const { return __aircraft; }
  
  /**
   * The client's current True Air Speed, in knots.
   * @sa groundSpeed().
   */
  inline int tas() const { return __tas; }
  
  /**
   * The client's flight rules - Ifr or Vfr.
   */
  inline const Pilot::FlightRules& flightRules() const { return __flightRules; }
  
  /**
   * The client's remarks.
   */
  inline const QString& remarks() const { return __remarks; }
  
  /**
   * The client's current heading.
   */
  inline unsigned heading() const { return __heading; }
  
  /**
   * The client's current flight status.
   * Flight status is determined by the current altitude, speed and distance
   * from airport (origin or destination).
   * 
   * TODO Climbing & descending status options.
   */
  inline Pilot::Status flightStatus() const { return __flightStatus; }
  
  /**
   * The client's baro setting.
   */
  inline const Pilot::Pressure& pressure() const { return __pressure; }
  
  /**
   * The client's route details. It includes planned origin and destination
   * airport, altitude and filled route.
   */
  inline const Pilot::Route& route() const { return __route; }
  
  /**
   * Prefiled only means that client has prefiled the flight plan, but
   * he did not log in yet.
   */
  inline bool isPrefiledOnly() const { return __prefiledOnly; }

private:
  /**
   * Adds origin/destination airports to the vectors.
   */
  void __updateAirports();
  
  /**
   * Checks whether pilot is departing, airborn or has just arrived.
   */
  void __setMyStatus();
  
  int                   __altitude;
  int                   __groundSpeed;
  QString               __squawk;
  QString               __aircraft;
  int                   __tas;
  Pilot::FlightRules    __flightRules;
  QString               __remarks;
  unsigned              __heading;
  Pilot::Status         __flightStatus;
  Pilot::Pressure       __pressure;
  Pilot::Route          __route;
  bool                  __prefiledOnly;

};

#endif // PILOT_H
