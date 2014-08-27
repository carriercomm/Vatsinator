/*
 * vatsinatorstyle.h
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

#ifndef VATSINATORSTYLE_H
#define VATSINATORSTYLE_H

#include <QtGlobal>
#ifndef Q_OS_ANDROID

# include <QProxyStyle>
# include <QFont>

class VatsinatorStyle : public QProxyStyle {
  Q_OBJECT

public:
  
  QFont boldFont();
  QFont h1Font();
  QFont h2Font();
  
#ifdef Q_OS_MAC
  void polish(QWidget*) override;
#endif
  
};

#endif // Q_OS_ANDROID

#endif // VATSINATORSTYLE_H
