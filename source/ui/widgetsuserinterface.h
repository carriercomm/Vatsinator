/*
 * widgetsuserinterface.h
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

#ifndef WIDGETSUSERINTERFACE_H
#define WIDGETSUSERINTERFACE_H

#include "ui/userinterface.h"
#include "vatsinatorapplication.h"

class AboutWindow;
class Airport;
class AtcListWindow;
class Client;
class DatabaseWindow;
class Fir;
class FlightListWindow;
class Pilot;
class MetarsWindow;
class NotificationEvent;
class SettingsWindow;
class VatsinatorWindow;

/**
 * This is the desktop implementation of UserInterface abstract class.
 * Widgets stands for QtWidgets module.
 */
class WidgetsUserInterface : public UserInterface {
  Q_OBJECT
  
public:
  explicit WidgetsUserInterface(QObject* parent = nullptr);
  virtual ~WidgetsUserInterface();
  
  void initialize() override;
  
  /**
   * \return Instance of the "About Vatsinator" window.
   */
  AboutWindow* aboutWindow();
  
  /**
   * \return Instance of the "Atc list" window.
   */
  AtcListWindow* atcListWindow();
  
  /**
   * \return Instance of the "VatsinatorDatabase" window.
   */
  DatabaseWindow* databaseWindow();
  
  /**
   * \return Instance of the "Flight list" window.
   */
  FlightListWindow* flightListWindow();
  
  /**
   * \return Instance of the "METARs" window.
   */
  MetarsWindow* metarsWindow();
  
  /**
   * \return Instance of the settings window.
   */
  SettingsWindow* settingsWindow();
  
  /**
   * \return Instance of the main Vatsinator window.
   */
  VatsinatorWindow* mainWindow();
  
public slots:
  /**
   * Shows the dialog that shows "Restart the application" dialog.
   * Called when user changes the language.
   */
  void showAppRestartDialog();
  
  void fatal(const QString& message) override;
  void warning(const QString& message) override;
  void statusError() override;
  void dataError() override;
  void showVatsimMessage(const QString& message) override;
  void showDetails(const Airport* airport) override;
  void showDetails(const Client* client) override;
  void showDetails(const Fir* fir) override;
  void showMetar(const QString& metar) override;
  void showStatsDialog() override;
  void ensureMainWindowIsActive() override;
  
private slots:
  /**
   * Shows the dialog that notifies user about the new version available.
   * We do not need to implement this on Android - Play handles auto-updates
   * for us.
   */
  void __showNewVersionDialog();

private:
  AboutWindow*          __aboutWindow;
  MetarsWindow*         __metarsWindow;
  DatabaseWindow*       __databaseWindow;
  AtcListWindow*        __atcListWindow;
  FlightListWindow*     __flightsListWindow;
  SettingsWindow*       __settingsWindow;
  VatsinatorWindow*     __vatsinatorWindow;
  
};

/**
 * You can use this function to access specific function of WidgetsUserInterface
 * implementation as well as check the current ui mode.
 */
inline WidgetsUserInterface* wui() {
  return qobject_cast<WidgetsUserInterface*>(vApp()->userInterface());
}

#endif // WIDGETSUSERINTERFACE_H
