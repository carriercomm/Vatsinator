/*
 * databasewindow.h
 * Copyright (C) 2013-2014  Michał Garapich <michal@garapich.pl>
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

#ifndef DATABASEWINDOW_H
#define DATABASEWINDOW_H

#include "ui/windows/basewindow.h"
#include "network/resourcemanager.h"
#include "ui/ui_databasewindow.h"

class DatabaseWindow : public BaseWindow, private Ui::DatabaseWindow {
  
  Q_OBJECT

public:
  DatabaseWindow(QWidget* parent = nullptr);
  
private slots:
  void __updateNumbers();
  void __updateDatabaseStatus(ResourceManager::VersionStatus status);
  
};

#endif // DATABASEWINDOW_H
