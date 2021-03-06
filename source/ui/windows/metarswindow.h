/*
    metarswindow.h
    Copyright (C) 2012-2015  Michał Garapich michal@garapich.pl

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


#ifndef METARSWINDOW_H
#define METARSWINDOW_H

#include <QVector>
#include <QWidget>

#include "ui/ui_metarswindow.h"
#include "vatsimdata/metar.h"

class VatsimDataHandler;

/**
 * \todo Refactor.
 */
class MetarsWindow : public QWidget, private Ui::MetarsWindow {
  Q_OBJECT

public:
  MetarsWindow(QWidget* parent = nullptr);

public slots:
  void show(QString icao);
  void metarRequested();
  
protected:
  void showEvent(QShowEvent* event);
  void keyPressEvent(QKeyEvent* event);

private:
  void __findAndSelectMetar(const QString& icao, bool fetchIfNotFound = true);

private slots:
  void __handleTextChange(const QString& text);
  void __handleNewMetars();
  void __enableButtons();
  
private:
  /* Keeps the ICAO of the metar that user's waiting for */
  QString __awaited;

};

#endif // METARSWINDOW_H
