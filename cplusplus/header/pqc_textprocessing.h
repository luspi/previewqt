#ifndef PQCTEXTPROCESSING_H
#define PQCTEXTPROCESSING_H

#include <QObject>

class PQCTextProcessing : public QObject {

    Q_OBJECT

public:
    static PQCTextProcessing& get() {
        static PQCTextProcessing instance;
        return instance;
    }
    ~PQCTextProcessing();

    PQCTextProcessing(PQCTextProcessing const&)     = delete;
    void operator=(PQCTextProcessing const&) = delete;

    Q_INVOKABLE QString prettifyJSON(QString json);

private:
    PQCTextProcessing();

};



#endif
