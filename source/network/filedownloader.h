/*
    filedownloader.h
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


#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QNetworkAccessManager>
#include <QObject>
#include <QQueue>
#include <QFile>
#include <QUrl>

class QNetworkReply;

/*
 * Use this class to download files from the internet.
 */
class FileDownloader : public QObject {
  Q_OBJECT

signals:
  
  /**
   * Emited when download is complete.
   * @param fileName Location of the downloaded file.
   */
  void finished(QString);
  
  /**
   * Emited when an error occurs.
   * @param erStr Error string.
   */
  void error(QString);
  
public:
  /**
   * Creates new FileDownloader instance.
   */
  FileDownloader(QObject* = 0);
  
  /**
   * If the requests queue is empty, downloads the given file
   * immediately. Otherwise, enqueues the url.
   */
  void fetch(const QUrl&);
  
  /**
   * Generates the temporary file name (with the absolute path)
   * from the given url.
   */
  QString fileNameForUrl(const QUrl&);
  
  /**
   * Returns true if there are any queries scheduled.
   */
  inline bool anyTasksLeft() const { return !__urls.isEmpty(); }

private slots:
  void __readyRead();
  void __finished();
  
private:
  void __startRequest();
  
  QQueue<QUrl>   __urls;
  QFile          __output;
  
  QNetworkAccessManager __nam;
  QNetworkReply*        __reply;
  
};

#endif // FILEDOWNLOADER_H
