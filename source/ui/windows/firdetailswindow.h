/*
    firdetailswindow.h
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


#ifndef FIRDETAILSWINDOW_H
#define FIRDETAILSWINDOW_H

#include "ui/windows/basewindow.h"

#include "ui/ui_firdetailswindow.h"
#include "singleton.h"

class Airport;
class Controller;
class Fir;
class Pilot;

class FirDetailsWindow :
    public BaseWindow,
    public Singleton< FirDetailsWindow >,
    private Ui::FirDetailsWindow {

  Q_OBJECT

public:
  FirDetailsWindow(QWidget* = nullptr);

public slots:
  void show(const Fir*);

private:
  void __updateModels(const Fir* = NULL);
  void __fillLabels(const Fir*);
  void __adjustTables();

  QString __currentICAO;
  
  /*
   * We don't need dataUpdated() slot here, as FirDetailsWindow
   * uses already created models and doesn't make any new one.
   * Therefore, all data updates (resets) are handled by the
   * views.
   */


};

#endif // FIRDETAILSWINDOW_H
