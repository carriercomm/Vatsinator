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
# include <QProxyStyle>
#else
# include <QGuiApplication>
#endif

#include <QFont>
#include <QMutex>
#include <iostream>

#include "singleton.h"

class AirlineDatabase;
class AirportDatabase;
class CacheFile;
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
   * First emitted in the application. No objects are created yet.
   */
  void initializing();

public:
  /**
   * Constructor gives argc & argv to the QApplication.
   */
  VatsinatorApplication(int&, char**);

  virtual ~VatsinatorApplication();
  
  UserInterface* userInterface();
  
  inline SettingsManager* settingsManager() { Q_ASSERT(__settingsManager); return __settingsManager; }
  inline const PluginManager* plugins() const { Q_ASSERT(__pluginManager); return __pluginManager; }
  inline VatsimDataHandler* vatsimDataHandler() { Q_ASSERT(__vatsimData); return __vatsimData; }
  inline const VatsimDataHandler* vatsimDataHandler() const { Q_ASSERT(__vatsimData); return __vatsimData; }
  
#ifdef GCC_VERSION_48
  [[noreturn]]
#endif
    static void terminate();

public slots:
  void restart();
  
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
  PluginManager*       __pluginManager;
  AirlineDatabase*     __airlineDatabase;
  AirportDatabase*     __airportDatabaase;
  FirDatabase*         __firDatabase;
  WorldMap*            __worldMap;
  VatsimDataHandler*   __vatsimData;
  LanguageManager*     __languageManager;
  ModuleManager*       __moduleManager;
  ResourceManager*     __resourceManager;
  StatsPurveyor*       __statsPurveyor;
  
  static QMutex        __mutex; /* For stdout */

};

auto vApp = []() -> VatsinatorApplication* {
  return qobject_cast<VatsinatorApplication*>(QCoreApplication::instance());
};

#endif // VATSINATORAPPLICATION_H
