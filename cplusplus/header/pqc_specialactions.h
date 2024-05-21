#ifndef PQCPROCESSONLY_H
#define PQCPROCESSONLY_H

#include <QString>

class PQCSpecialActions {

public:
    PQCSpecialActions();

    static void processOnly(QString path, int fileNumInside);

    static QString getSelectedFile();
    static QString clipboardGet();
    static void clipboardSet(QString val);

private:
    static QString getSelectedFile_dolphin();

};

#endif // PQCPROCESSONLY_H
