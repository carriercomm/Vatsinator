/*
    filemanager.cpp
    Copyright (C) 2013  Michał Garapich michal@garapich.pl

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

#include <QtCore>

#include "storage/cachefile.h"
#include "config.h"
#include "vatsinatorapplication.h"

#include "storage/filemanager.h"


FileManager::FileManager() {
  qDebug("FileManager: local data location: %s", qPrintable(localDataPath()));
  
  // ensure that our data directory exists
  QDir dir(localDataPath());
  if (!dir.exists()) {
    qDebug("FileManager: creating directory %s", qPrintable(localDataPath()));
    dir.mkpath(".");
  }
}

void
FileManager::cacheData(const QString& _fileName, const QString& _data) {
  CacheFile cache(_fileName);
  cache.open(QIODevice::WriteOnly | QIODevice::Truncate);
  cache.write(_data.toUtf8());
  cache.close();
}

bool
FileManager::moveToCache(const QString& _source, const QString& _destination) {
  QFile file(_source);
  if (!file.open(QIODevice::ReadWrite)) {
    qWarning("FileManager: failed to access file %s", qPrintable(_source));
    return false;
  }
  
  file.close();
  
  CacheFile oldCache(_destination);
  QFileInfo info(oldCache);
  QDir(info.path()).mkpath(".");
  
  if (oldCache.exists())
    oldCache.remove();
  
  bool result = file.rename(oldCache.fileName());
  if (result)
    qDebug("FileManager: cached file %s", qPrintable(_destination));
  else
    qWarning("FileManager: failed caching file %s", qPrintable(_destination));
  
  return result;
}

QString
FileManager::staticPath(FileManager::StaticDir _d) {
  switch (_d) {
    case Plugins:
#if defined Q_OS_ANDROID
      return "assets:/plugins";
#elif defined Q_OS_DARWIN
      return QCoreApplication::applicationDirPath() % "../Resources/plugins";
#else
      return QString(VATSINATOR_PREFIX) % "plugins";
#endif
      
    case Translations:
#if defined Q_OS_ANDROID
      return "assets:/translations";
#elif defined Q_OS_DARWIN
      return QCoreApplication::applicationDirPath() % "/../Resources/translations";
#else
      return QString(VATSINATOR_PREFIX) % "translations";
#endif
    
    default:
      Q_ASSERT_X(false, "getting static path", "No such file!");
      return QString();
  }
}

QString
FileManager::path(const QString& _f) {
  
  QFile tryLocal(localDataPath() % _f);
  if (tryLocal.exists()) {
    qDebug("FileManager: file %s loaded from %s.",
           qPrintable(_f), qPrintable(tryLocal.fileName()));
    return tryLocal.fileName();
  } else {
    return
#if defined Q_OS_ANDROID
      "assets:/"
#elif defined Q_OS_DARWIN // on MacOS look for the file in the bundle
      QCoreApplication::applicationDirPath() % "/../Resources/"
#else 
      static_cast<QString>(VATSINATOR_PREFIX)
#endif
      % _f;
  }
}

QString
FileManager::localDataPath() {
  static const QString LocalDataLocation =
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QDir::cleanPath(
        QStandardPaths::writableLocation(QStandardPaths::DataLocation)
      % QDir::separator()
      % "Vatsinator"
    ) % QDir::separator()
#else
    QDir::cleanPath(
      QDesktopServices::storageLocation(QDesktopServices::DataLocation)
      % QDir::separator()
      % "Vatsinator"
    ) % QDir::separator()
#endif
    ;
    
  return LocalDataLocation;
}

QString
FileManager::cachePath() {
  static const QString CacheLocation = 
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QDir::cleanPath(
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation)
      % QDir::separator()
      % "Vatsinator"
    ) % QDir::separator()
#else
    QDir::cleanPath(
      QDesktopServices::storageLocation(QDesktopServices::CacheLocation)
      % QDir::separator()
      % "Vatsinator"
    ) % QDir::separator()
#endif
    ;
  
  return CacheLocation;
}
