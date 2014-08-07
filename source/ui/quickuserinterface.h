/*
 * quickuserinterface.h
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

#ifndef QUICKUSERINTERFACE_H
#define QUICKUSERINTERFACE_H

#include "ui/userinterface.h"

class QQuickView;

/**
 * This is the mobile version of the user interface, based on QtQuick.
 */
class QuickUserInterface : public UserInterface {
  Q_OBJECT

public:
  explicit QuickUserInterface(QObject* = nullptr);
  virtual ~QuickUserInterface();
  
  void initialize() override;
  
public slots:
  void fatal(const QString&) override;
  void warning(const QString&) override;
  void statusError() override;
  void dataError() override;
  void showVatsimMessage(const QString&);
  void showDetails(const Airport*);
  void showDetails(const Client*);
  void showDetails(const Fir*);
  void showMetar(const QString&);
  
private:
  QQuickView* __view;
  
};

#endif // QUICKUSERINTERFACE_H
