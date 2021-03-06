/*
    flightlistwindow.h
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


#ifndef FLIGHTLISTWINDOW_H
#define FLIGHTLISTWINDOW_H

#include "ui/windows/basewindow.h"
#include "ui/ui_flightlistwindow.h"

class FlightListWindow :
    public BaseWindow,
    private Ui::FlightListWindow {

  Q_OBJECT

public:
  FlightListWindow(QWidget* parent = nullptr);

protected:
  void showEvent(QShowEvent* event) override;

private slots:
  void __handleDoubleClicked(const QModelIndex& index);

};

#endif // FLIGHTLISTWINDOW_H
