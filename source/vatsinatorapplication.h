/*
    vatsinatorapplication.h
    Copyright (C) 2012  Michał Garapich garrappachc@gmail.com
    
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

#include <QApplication>
#include <QTimer>

#include "singleton.h"

class AirportsDatabase;
class FirsDatabase;
class UserInterface;
class VatsimDataHandler;
class SettingsManager;
class WorldMap;
class HttpHandler;
class ModulesManager;

class VatsinatorApplication :
		public QApplication,
		public Singleton< VatsinatorApplication > {
	
	/*
	 * This class handles the whole Vatsinator application and has
	 * only one instance.
	 */
	
	Q_OBJECT
	
public:
	/**
	 * Constructor gives argc & argv to the QApplication.
	 */
	VatsinatorApplication(int&, char**);
	
	virtual ~VatsinatorApplication();
	
	static void quit();
	static void alert(const QString&);
	
	static void emitGLInitialized();
	
	VatsimDataHandler& getData() { return *__vatsimData; }
	
signals:
	void glInitialized();
	void dataUpdated();
	void metarsRefreshRequested();

public slots:
	void refreshData();
	
private slots:
	void __statusFileUpdated(const QString&);
	void __dataFileUpdated(const QString&);
	void __showDataAlert();
	
private:
	void __emitGLInitialized();
	void __fetchStatusFile();
	
	HttpHandler *		__httpHandler;
	AirportsDatabase *	__airportsData;
	FirsDatabase * 		__firsData;
	WorldMap *		__worldMap;
	VatsimDataHandler *	__vatsimData;
	SettingsManager *	__settingsManager;
	ModulesManager *	__modulesManager;
	UserInterface *		__userInterface;
	QTimer 			__timer;
	
};

#endif // VATSINATORAPPLICATION_H

struct Metar;