#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class Settings
{
public:
    enum {OUTDIR, SRCDIR, SRCPATH, EN_EXTFILTER, EXTFILTER, EN_IGNOREFILTER,
          IGNOREFILTER, MODE, EN_MAXFILECOUNT, MAXFILECOUNT, EN_MINFREESPACE,
          MINFREESPACE, EN_LIMIT, LIMIT, EN_MAXDST, MAXDST, KEYS_COUNT};

    Settings();
    ~Settings();

    bool getBool(int index);
    bool getBool(int index, bool defaultValue);
    double getDouble(int index);
    int getInt(int index);
    QString getString(int index);
    void setBool(int index, bool value);
    void setDouble(int index, double value);
    void setInt(int index, int value);
    QString getOutputDir();
    void setString(int index, QString value);
    void sync();
private:
    QString keys[KEYS_COUNT];
    QSettings *qSettings;
};

#endif // SETTINGS_H
