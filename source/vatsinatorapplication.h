/*
    vatsinatorapplication.h
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


#ifndef VATSINATORAPPLICATION_H
#define VATSINATORAPPLICATION_H

#include <QtGlobal>

#ifndef Q_OS_ANDROID
# include <QApplication>
#else
# include <QGuiApplication>
#endif

#include <QFont>
#include <QMutex>


class AirlineDatabase;
class AirportDatabase;
class CacheFile;
class DecisionEvent;
class FileManager;
class FirDatabase;
class LanguageManager;
class ModuleManager;
class PluginManager;
class ResourceManager;
class SettingsManager;
class StatsPurveyor;
class UserInterface;
class VatsimDataHandler;
class WorldMap;

/**
 * The VatsinatorApplication wraps the whole Vatsinator instance
 * in a convenient and simple way. It holds singleton instances and
 * controls whole application initialization process.
 */
class VatsinatorApplication : public
#ifndef Q_OS_ANDROID
    QApplication
#else
    QGuiApplication
#endif
{

  Q_OBJECT
  
signals:
  /**
   * First signal emitted in the application.
   * When this signal is emitted, all singletons are already instantiated,
   * but they are not initialized yet.
   */
  void initializing();

public:
  /**
   * Constructor forwards argc & argv to the QApplication.
   */
  VatsinatorApplication(int& argc, char** argv);

  /**
   * Cleans the resources up.
   */
  virtual ~VatsinatorApplication();
  
  /**
   * Custom event handler.
   */
  bool event(QEvent* event) override;
  
  /**
   * Gets the UserInterface singleton.
   */
  inline UserInterface* userInterface() {
    Q_ASSERT(__userInterface);
    return __userInterface;
  }
  
  /**
   * Gets the SettingsManager singleton.
   */
  inline SettingsManager* settingsManager() {
    Q_ASSERT(__settingsManager);
    return __settingsManager;
  }
  
  /**
   * Gets the VatsimDataHandler singleton.
   */
  inline VatsimDataHandler* vatsimDataHandler() {
    Q_ASSERT(__vatsimData);
    return __vatsimData;
  }
  
  /**
   * Gets the VatsimDataHandler singleton const pointer.
   */
  inline const VatsimDataHandler* vatsimDataHandler() const {
    Q_ASSERT(__vatsimData);
    return __vatsimData;
  }
  
  /**
   * Gets the StatsPurveyor singleton.
   */
  inline StatsPurveyor* statsPurveyor() {
    Q_ASSERT(__statsPurveyor);
    return __statsPurveyor;
  }
  
  /**
   * Gets the ResourceManager singleton.
   */
  inline ResourceManager* resourceManager() {
    Q_ASSERT(__resourceManager);
    return __resourceManager;
  }
  
  /**
   * Gets the AirlineDatabase singleton.
   */
  inline AirlineDatabase* airlineDatabase() {
    Q_ASSERT(__airlineDatabase);
    return __airlineDatabase;
  }
  
  /**
   * Gets the AirportDatabase singleton.
   */
  inline AirportDatabase* airportDatabase() {
    Q_ASSERT(__airlineDatabase);
    return __airportDatabaase;
  }
  
  /**
   * Gets the FirDatabase singleton.
   */
  inline FirDatabase* firDatabase() {
    Q_ASSERT(__firDatabase);
    return __firDatabase;
  }

public slots:
  /**
   * Restarts the application.
   */
  void restart();
  
protected:
  /**
   * Handles the DecisionEvent.
   * THe only recognized context here is "statistics" which indicates
   * whether user agreed to send stats to Vatsinator servers or not.
   */
  virtual void userDecisionEvent(DecisionEvent* event);
  
private slots:
  /**
   * Initialize the application.
   * This slot is connected to the initializing() signal.
   */
  void __initialize();
  
private:
  
  UserInterface*       __userInterface;
  FileManager*         __fileManager;
  SettingsManager*     __settingsManager;
  AirlineDatabase*     __airlineDatabase;
  AirportDatabase*     __airportDatabaase;
  FirDatabase*         __firDatabase;
  WorldMap*            __worldMap;
  VatsimDataHandler*   __vatsimData;
  LanguageManager*     __languageManager;
  ModuleManager*       __moduleManager;
  ResourceManager*     __resourceManager;
  StatsPurveyor*       __statsPurveyor;

};

auto vApp = []() -> VatsinatorApplication* {
  return qobject_cast<VatsinatorApplication*>(QCoreApplication::instance());
};

#endif // VATSINATORAPPLICATION_H
