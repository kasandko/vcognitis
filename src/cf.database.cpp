#include "cf.database.h"

#include <QSqlDatabase>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QFile>
#include <QSqlQuery>
#include <QSqlError>

bool CFDatabase::init(QString userId)
{
    QSqlDatabase::addDatabase(DB_TYPE);
    QSqlDatabase::database().setDatabaseName(fullPath(userId));
    if (!QSqlDatabase::database().open()) {
        qCritical() << "Could not open database: " << QSqlDatabase::database().lastError();
        return false;
    }

    // We need second file for init interest tables
    return execFile(":/db_init.sql") && execFile(":/interest_db_init.sql");
}

/**
 * @brief Reads SQL script and executes it
 * NB! Fails if there are embedded ';' in SQL strings
 */
bool CFDatabase::execFile(const QString &filePath)
{
    QFile scriptFile(filePath);
    if (!scriptFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return  false;
    }

    QTextStream ts(&scriptFile);
    ts.setCodec("UTF-8");
    QStringList sqlStatements = ts.readAll().split(';', QString::SkipEmptyParts);
    int successCount = 0;

    for (const QString& statement : sqlStatements) {
        if (statement.trimmed() != "") {
            QSqlQuery query;
            if (query.exec(statement)) {
                successCount++;
            } else {
                qDebug() << "Failed:" << statement << "\nReason:" << query.lastError();
            }
        }
    }
    return successCount;
}

QString CFDatabase::fullPath(const QString &userId)
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!QDir(path).exists()) {
        QDir().mkpath(path);
    }
    return path + QDir::separator() + DB_NAME.arg(userId);
}
