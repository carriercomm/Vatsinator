/*
    scrolllabel.cpp
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

#include <QtWidgets>

#include "scrolllabel.h"

static constexpr int ScrollLabelMargin = 5;

ScrollLabel::ScrollLabel(QWidget* parent) :
    QWidget(parent),
    __textPosition(0) {
  
  connect(&__timer, SIGNAL(timeout()),
          this,     SLOT(__timerTimeout()));
  __timer.setInterval(3000);
}

void
ScrollLabel::setText(const QString& text) {
  QString temp(__text);
  __text = text;
  
  if (temp != __text)
    __updateText();
}

void
ScrollLabel::paintEvent(QPaintEvent* event) {
  QPainter p(this);
  
  p.drawStaticText(QPointF(ScrollLabelMargin - __textPosition,
                           (height() - __textSize.height()) / 2),
                   __staticText);
  
  Q_UNUSED(event);
}

void
ScrollLabel::__updateText() {
  __timer.stop();
  __timer.setInterval(3000);
  __textPosition = 0;
  
  __staticText.setText(__text);
  __staticText.prepare(QTransform(), font());
  
  __textSize = QSize(fontMetrics().width(__staticText.text()),
                     fontMetrics().height());
  
  if (__textSize.width() + (ScrollLabelMargin * 2) > width())
    __timer.start();
  
  update();
}

void
ScrollLabel::__timerTimeout() {
  if (__textPosition == 0) {
    __timer.setInterval(40);
    __textPosition += 1;
  } else {
    if (__textPosition > __textSize.width()) {
      __timer.setInterval(3000);
      __textPosition = 0;
    } else {
      __textPosition += 1;
    }
  }
  
  update();
}
