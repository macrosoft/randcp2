#include "settings.h"

Settings::Settings() {
    qSettings = new QSettings("randcp2.ini",QSettings::IniFormat);
    keys[OUTDIR] = "outdir";
    keys[SRCDIR] = "srcdir";
    keys[SRCPATH] = "srcpath";
    keys[EN_EXTFILTER] = "enextfilter";
    keys[EXTFILTER] = "extfilter";
    keys[EN_IGNOREFILTER] = "enignorefilter";
    keys[IGNOREFILTER] = "ignorefilter";
    keys[MODE] = "mode";
    keys[EN_MAXFILECOUNT] = "enablemaxfilecount";
    keys[MAXFILECOUNT] = "maxfilecount";
    keys[EN_MINFREESPACE] = "enminfreespace";
    keys[MINFREESPACE] = "minfreespace";
    keys[EN_LIMIT] = "enlimit";
    keys[SIZE_LIMIT] = "limit";
    keys[EN_MAXDST] = "enmaxdst";
    keys[MAXDST] = "maxdst";
}

Settings::~Settings() {
    qSettings->deleteLater();
}

bool Settings::getBool(int index) {
    QString key = keys[index];
    return qSettings->value(key).toBool();
}

double Settings::getDouble(int index) {
    QString key = keys[index];
    return qSettings->value(key).toDouble();
}

int Settings::getInt(int index) {
    return getInt(index, 0);
}

int Settings::getInt(int index, int defaultValue) {
    QString key = keys[index];
    return qSettings->value(key, defaultValue).toInt();
}

QString Settings::getOutputDir() {
    QString outDir = qSettings->value(keys[OUTDIR]).toString();
#ifdef Q_OS_WIN
    if (outDir.length() == 3 and outDir[1] == ':')
        outDir.remove(2,1);
#endif
    return outDir;
}

QString Settings::getString(int index) {
    QString key = keys[index];
    return qSettings->value(key).toString();
}

void Settings::setBool(int index, bool value) {
    QString key = keys[index];
    qSettings->setValue(key, value);
}

void Settings::setDouble(int index, double value) {
    QString key = keys[index];
    qSettings->setValue(key, value);
}

void Settings::setInt(int index, int value) {
    QString key = keys[index];
    qSettings->setValue(key, value);
}

void Settings::setString(int index, QString value) {
    QString key = keys[index];
    qSettings->setValue(key, value);
}

void Settings::sync() {
    qSettings->sync();
}

void Settings::setMode(bool mode) {
    if (mode)
        setInt(MODE, SHUFFLE_MODE);
    else
        setInt(MODE, SYNCHRONIZE_MODE);
}
