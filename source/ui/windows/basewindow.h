/*
 * basewindow.h
 * Copyright (C) 2013  Michał Garapich <michal@garapich.pl>
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

#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#include <QWidget>

class BaseWindow : public QWidget {
  
  /**
   * BaseWindow is a base class for all Vatsinator's subwindows.
   * It ensures that all windows open on the correct screen,
   * in the correct place.
   */
  
  Q_OBJECT
  
public:
  BaseWindow(QWidget* = nullptr, Qt::WindowFlags = 0);

protected:
  void showEvent(QShowEvent*);
  
private:
  bool __windowSetup; /* Indicates whether the window paremeters
                         were set up in showEvent() */

};

#endif // BASEWINDOW_H
