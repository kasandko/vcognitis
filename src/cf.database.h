/****************************************************************************
**
** Copyright © 2020 Barbatum™.
** All rights reserved.
** Contact: support@barbatum.ru
**
**
** This file is part of VCognitis project.
**
** VCognitis is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** VCognitis is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with VCognitis.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef CFDATABASE_H
#define CFDATABASE_H

#include <QString>

namespace CFDatabase {

const QString DB_TYPE = "QSQLITE";
const QString DB_NAME = "cf_db%1.sqlite3";

bool init(QString userId);
bool execFile(const QString &filePath);
QString fullPath(const QString &userId);

}

#endif // CFDATABASE_H