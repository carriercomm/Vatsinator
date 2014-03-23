/*
 * notam.h
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

#ifndef NOTAM_H
#define NOTAM_H

#include <QString>
#include <QDateTime>

class Notam {
  
  /**
   * This is class that represents a single NOTAM message.
   */
  
public:
  
  /**
   * NOTAM type enum.
   */
  enum Type {
    New,
    Replacement,
    Cancellation
  };
  
  /**
   * @param ident The NOTAM ident.
   */
  Notam(QString);
  
  /**
   * @param ident The NOTAM ident.
   * @param icao The NOTAM icao.
   * @param notam The NOTAM message.
   * @param url The URL where user can go to to see full message.
   * @param from NOTAM effective from.
   * @param to NOTAM effective to. Leave null if permament.
   * @param type NOTAM type.
   */
  Notam(QString, QString, QString, QString, QDateTime, QDateTime, Type);
  
  void setIcao(const QString&);
  void setNotam(const QString&);
  void setUrl(const QString&);
  void setFrom(const QDateTime&);
  void setTo(const QDateTime&);
  void setType(Notam::Type);
  
  /**
   * NOTAM ident.
   */
  inline const QString& ident() const {
    return __ident;
  }
  
  /**
   * NOTAM icao.
   */
  inline const QString& icao() const {
    return __icao;
  }
  
  /**
   * NOTAM message.
   */
  inline const QString& notam() const {
    return __notam;
  }
  
  /**
   * URL to where user can see the full message. Empty if no
   * URL needed or specified.
   */
  inline const QString& url() const {
    return __url;
  }
  
  /**
   * NOTAM effective from.
   */
  inline const QDateTime& from() const {
    return __from;
  }
  
  /**
   * NOTAM effective to. Null if permament.
   */
  inline const QDateTime& to() const {
    return __to;
  }
  
  /**
   * NOTAM type.
   */
  inline Notam::Type type() const {
    return __type;
  }
  
private:
  QString       __ident;
  QString       __icao;
  QString       __notam;
  QString       __url;
  QDateTime     __from;
  QDateTime     __to;
  Type          __type;
};

#endif // NOTAM_H
