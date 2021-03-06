/*
 * euroutenotamprovider.cpp
 * Copyright (C) 2014-2015  Michał Garapich <michal@garapich.pl>
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

#include <QtCore>

#include "network/filedownloader.h"
#include "storage/cachefile.h"
#include "storage/filemanager.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "euroutenotamprovider.h"

static const QString EurouteNotamXmlUrl =
    QStringLiteral("http://notams.euroutepro.com/notams.xml");
static const QString EurouteLink =
    QStringLiteral("http://notams.euroutepro.com/doc/rss.php");

namespace {
  Notam::Type typeFromLetter(const QString& letter) {
    if (letter == "R")
      return Notam::Replacement;
    else if (letter == "C")
      return Notam::Cancellation;
    else
      return Notam::New;
  }
}

EurouteNotamProvider::EurouteNotamProvider(QObject* parent) : NotamProvider(parent) {
  qDebug("EUroute URL: %s", qPrintable(EurouteNotamXmlUrl));
  connect(vApp()->vatsimDataHandler(), &VatsimDataHandler::vatsimDataUpdated,
          this, &EurouteNotamProvider::__checkXmlUpToDate);
  __checkXmlUpToDate();
}

EurouteNotamProvider::~EurouteNotamProvider() {
  qDeleteAll(__notamModels);
}

void
EurouteNotamProvider::fetchNotam(const QString& icao) {
  if (!__lastUpdate.isValid()) {
    /* We don't have valid XML yet, lets wait for it */
    __icaoRequest = icao;
    return;
  }
  
  if (__notamModels.contains(icao)) {
    emit notamReady(__notamModels[icao]);
    return;
  }
  
  NotamListModel* nlm = new NotamListModel(icao);
  __notamModels.insert(icao, nlm);
  __fillNotamListModel(nlm);
  emit notamReady(nlm);
}

QString
EurouteNotamProvider::providerInfo() const {
  return tr("NOTAMs by <a href=\"%1\">EUroute</a>").arg(EurouteLink);
}

void
EurouteNotamProvider::__fetchXml() {
  FileDownloader* fd = new FileDownloader();
  connect(fd, &FileDownloader::finished, this, &EurouteNotamProvider::__xmlReady);
  
  fd->fetch(EurouteNotamXmlUrl);
}

void
EurouteNotamProvider::__readXmlHeader() {
  CacheFile file("euroute-notams.xml");
  if (!file.open()) {
    qCritical("EurouteNotamProvider: failed to open file %s for reading", qPrintable(file.fileName()));
    __lastUpdate = QDateTime();
    return;
  }
  
  QXmlStreamReader reader;
  reader.setDevice(&file);
  while(!reader.atEnd() && !reader.hasError()) {
    auto token = reader.readNext();
    if (token == QXmlStreamReader::StartElement &&
        reader.name() == "general") {
      auto attrs = reader.attributes();
      if (attrs.hasAttribute("last_update")) {
        __lastUpdate = QDateTime::fromString(attrs.value("last_update").toString(), "yyyy-MM-dd hh:mm:ss");
        qDebug("EurouteNotamProvider: last update: %s", qPrintable(__lastUpdate.toString()));
      } else {
        qWarning("EurouteNotamProvider: error: no last-update attribute");
        __lastUpdate = QDateTime();
      }
      
      break;
    }
  }
  
  file.close();
  
  if (reader.hasError()) {
    qCritical("EurouteNotamProvider: error parsing XML file: %s", qPrintable(reader.errorString()));
    __lastUpdate = QDateTime();
  }
}

void
EurouteNotamProvider::__fillNotamListModel(NotamListModel* model) {
  CacheFile file("euroute-notams.xml");
  if (!file.open()) {
    qCritical("EurouteNotamProvider: failed to open file %s for reading", qPrintable(file.fileName()));
    return;
  }
  
  QXmlStreamReader reader;
  reader.setDevice(&file);
  while(!reader.atEnd() && !reader.hasError()) {
    auto token = reader.readNext();
    if (token == QXmlStreamReader::StartElement && reader.name() == "notam") {
      auto attrs = reader.attributes();
      if (attrs.hasAttribute("ident") && attrs.hasAttribute("A") &&
          attrs.value("A").toString() == model->icao()) {
        
        QDateTime from = QDateTime::fromString(attrs.value("B").toString(), "yyyy-MM-dd hh:mm:ss");
        Notam::CFlag cflag = Notam::None;
        QDateTime to;
        if (!attrs.hasAttribute("C_flag") || attrs.value("C_flag").toString().isEmpty()) {
          to = QDateTime::fromString(attrs.value("C").toString(), "yyyy-MM-dd hh:mm:ss");
        } else if (attrs.value("C_flag").toString() == "EST") {
          to = QDateTime::fromString(attrs.value("C").toString(), "yyyy-MM-dd hh:mm:ss");
          cflag = Notam::Est;
        } else if (attrs.value("C_flag").toString() == "PERM") {
          cflag = Notam::Perm;
        }
        
        model->addNotam(Notam(
          attrs.value("ident").toString(),
          attrs.value("A").toString(),
          attrs.value("E").toString(),
          attrs.value("url").toString(),
          from, to,
          attrs.value("D").toString(),
          cflag,
          typeFromLetter(attrs.value("type").toString())
        ));
      }
    }
  }
  
  file.close();
  model->sort(0);
}

void
EurouteNotamProvider::__xmlReady(QString fileName) {
  Q_ASSERT(qobject_cast<FileDownloader*>(sender()));
  
  FileManager::moveToCache(fileName, "euroute-notams.xml");
  __readXmlHeader();
  
  if (__lastUpdate.isValid() && !__icaoRequest.isEmpty()) {
    fetchNotam(__icaoRequest);
    __icaoRequest = QString();
  }
  
  sender()->deleteLater();
}

void
EurouteNotamProvider::__checkXmlUpToDate() {
  CacheFile file("euroute-notams.xml");
  if (!file.exists()) {
    __fetchXml();
    return;
  }
  
  if (!__lastUpdate.isValid()) {
    __readXmlHeader();
    if (!__lastUpdate.isValid()) {
      __fetchXml();
      return;
    } else if (!__icaoRequest.isEmpty()) {
      fetchNotam(__icaoRequest);
      __icaoRequest = QString();
    }
  }
  
  if (__lastUpdate.secsTo(QDateTime::currentDateTimeUtc()) > 86400) { // 24 h
    __fetchXml();
    return;
  }
}
