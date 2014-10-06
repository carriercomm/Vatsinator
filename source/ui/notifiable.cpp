/*
 * notifiable.cpp
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

#include <QtCore>

#include "events/notificationevent.h"
#include "ui/userinterface.h"
#include "vatsinatorapplication.h"

#include "notifiable.h"

void
Notifiable::notifyError(const QString& message) {
  NotificationEvent* e = new NotificationEvent(NotificationEvent::Fatal, message);
  QCoreApplication::postEvent(vApp()->userInterface(), e, Qt::HighEventPriority);
}

void
Notifiable::notifyWarning(const QString& message) {
  NotificationEvent* e = new NotificationEvent(NotificationEvent::Warning, message);
  QCoreApplication::postEvent(vApp()->userInterface(), e);
}

void
Notifiable::notifyInfo(const QString& message) {
  NotificationEvent* e = new NotificationEvent(NotificationEvent::Information, message);
  QCoreApplication::postEvent(vApp()->userInterface(), e);
}
