/*
 * resourcemanager.cpp
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

#include <QtGui>

#include "network/filedownloader.h"
#include "network/plaintextdownloader.h"

#include "storage/filemanager.h"
#include "storage/settingsmanager.h"

#include "netconfig.h"
#include "vatsinatorapplication.h"

#include "resourcemanager.h"
#include "defines.h"

// start running after two seconds
static const int StartDelay = 2 * 1000;

// manifest file name, both on local storage and on the server
static const QString ManifestFileName = "Manifest";

namespace {
  
  /**
   * Moves the file.
   */
  bool moveFile(const QString& _oldLocation, const QString& _newLocation) {
    QFile file(_oldLocation);
    if (!file.open(QIODevice::ReadWrite)) {
      return false;
    }
  
    file.close();
    
    QFile oldFile(_newLocation);
    if (oldFile.exists())
      oldFile.remove();
    
    bool result = file.rename(_newLocation);
    if (result)
      VatsinatorApplication::log("ResourceManager: moved file %s -> %s", qPrintable(_oldLocation), qPrintable(_newLocation));
    else
      VatsinatorApplication::log("ResourceManager: moved file %s -> %s", qPrintable(_oldLocation), qPrintable(_newLocation));
    
    return result;
  }
  
}

ResourceManager::ResourceManager(QObject* _parent) :
    QObject(_parent) {
  
  qRegisterMetaType<ResourceManager::VersionStatus>("ResourceManager::VersionStatus");
  
  connect(this, SIGNAL(vatsinatorVersionChecked(ResourceManager::VersionStatus)),
                SLOT(__checkDatabase(ResourceManager::VersionStatus)));
  
  QTimer::singleShot(StartDelay, this, SLOT(__fetchVersion()));
}

ResourceManager::~ResourceManager() {}

bool
ResourceManager::__versionActual(const QString& _version1, const QString& _version2) {
  auto ver1 = _version1.split(QRegExp("\\D+"));
  auto ver2 = _version2.split(QRegExp("\\D+"));
  
  for (int i = 0; i < ver1.size() && i < ver2.size(); ++i) {
    if (ver1[i].toInt() < ver2[i].toInt())
      return false;
    
    if (ver1[i].toInt() > ver2[i].toInt())
      return true;
  }
  
  return true;
}

void
ResourceManager::__downloadManifest() {
  FileDownloader* fd = new FileDownloader();
  
  connect(fd,   SIGNAL(finished(QString)),
          this, SLOT(__handleManifest(QString)));
  connect(fd,   SIGNAL(finished(QString)),
          fd,   SLOT(deleteLater()));
  connect(fd,   SIGNAL(error(QString)),
          this, SLOT(__manifestError()));
  connect(fd,   SIGNAL(error(QString)),
          fd,   SLOT(deleteLater()));
  
  fd->fetch(QUrl(QString(NetConfig::Vatsinator::repoUrl()) % ManifestFileName));
  emit databaseStatusChanged(Updating);
}

void
ResourceManager::__moveFiles() {
  
  
  emit databaseStatusChanged(Updated);
}

void
ResourceManager::__fetchVersion() {
  if (SM::get("network.version_check").toBool()) {
    PlainTextDownloader* fetcher = new PlainTextDownloader();
    
    connect(fetcher,      SIGNAL(finished(QString)),
            this,         SLOT(__parseVersion(QString)));
    
    fetcher->fetchData(QString(NetConfig::Vatsinator::repoUrl()) % "VERSION");
  }
}

void
ResourceManager::__parseVersion(QString _versionString) {
  bool actual = __versionActual(QString(VATSINATOR_VERSION), _versionString);
  
  VatsinatorApplication::log("ResourceManager: version(%1) %2 version(%3)",
                             VATSINATOR_VERSION,
                             actual ? ">=" : "<",
                             qPrintable(_versionString.simplified()));
  
  if (!actual)
    emit outdated();
  
  emit vatsinatorVersionChecked(actual ? Updated : Outdated);
  
  sender()->deleteLater();
}

void
ResourceManager::__checkDatabase(ResourceManager::VersionStatus _status) {
  if (_status == ResourceManager::Outdated)
    emit databaseStatusChanged(Unknown);
  
  QFile manifest(FileManager::path(ManifestFileName));
  
  VatsinatorApplication::log("ResourceManager: Manifest file: %s", qPrintable(manifest.fileName()));
  
  if (manifest.open(QIODevice::ReadOnly)) {
    QDate today = QDate::currentDate();
    QDate when = QDate::fromString(manifest.readLine().simplified(), "yyyyMMdd");
    if (when.daysTo(today) < 7) {
      emit databaseStatusChanged(Updated);
    } else {
      __downloadManifest();
    }
    
    manifest.close();
  }
}

void
ResourceManager::__handleManifest(QString _fileName) {
  __manifestLocation = _fileName;
  
  if (moveFile(_fileName, FileManager::path(ManifestFileName, true)))
    emit databaseStatusChanged(Updated);
  else
    emit databaseStatusChanged(Outdated);
}

void
ResourceManager::__manifestError() {
  emit databaseStatusChanged(Outdated);
}
