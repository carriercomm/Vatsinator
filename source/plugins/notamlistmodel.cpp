/*
 * notamlistmodel.cpp
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

#include <algorithm>
#include <QtGui>

#include "ui/models/roles.h"

#include "notamlistmodel.h"

NotamListModel::NotamListModel(QString icao, QObject* parent) :
  QAbstractTableModel(parent),
  __icao(qMove(icao)) {}

void
NotamListModel::addNotam(Notam notam) {
  __notams << qMove(notam);
}

QVariant
NotamListModel::data(const QModelIndex& index, int role) const {
  if (index.column() != 0)
    return QVariant();
  
  const Notam& notam = __notams.at(index.row());
  
  switch (role) {
    case Qt::DisplayRole:
      return QString("%1 %2 %3").arg(
          notam.icao(),
          notam.ident(),
          notam.notam()
        );
      
    case Qt::ToolTipRole: {
      QString to;
      if (notam.to().isValid()) {
        to = notam.to().toString("yyyy-MM-dd hh:mm:ss");
        if (notam.cflag() == Notam::Est)
          to += " <strong>EST</strong>";
      } else {
        to = "<strong>PERM</strong>";
      }
      
      if (!notam.diurnal().isEmpty())
        to += "; " + notam.diurnal();
      
      return QString("<p style='white-space:nowrap'>Effective from %1 until %2</p>").arg(
        notam.from().toString("yyyy-MM-dd hh:mm:ss"),
        to
      );
    }
    
    case Qt::BackgroundRole:
      switch (notam.type()) {
        
        case Notam::Cancellation:
          return QBrush(QColor(255, 177, 177));
          
        default:
          return QVariant();
      }
    
    case UrlRole:
      return notam.url();
      
    default:
      return QVariant();
  }
}

int
NotamListModel::rowCount(const QModelIndex& parent) const {
  Q_UNUSED(parent);
  return __notams.size();
}

int
NotamListModel::columnCount(const QModelIndex& parent) const {
  Q_UNUSED(parent);
  return 1;
}

void
NotamListModel::sort(int column, Qt::SortOrder order) {
  Q_UNUSED(order);
  Q_UNUSED(column);
  std::sort(__notams.begin(), __notams.end());
}
