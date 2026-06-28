#include <pqc_textprocessing.h>
#include <QJsonDocument>

PQCTextProcessing::PQCTextProcessing() {}

PQCTextProcessing::~PQCTextProcessing() {}

QString PQCTextProcessing::prettifyJSON(QString json) {

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &error);

    if(error.error != QJsonParseError::NoError) {
        qWarning() << "ERROR parsing JSON:" << error.errorString();
        return json;
    }

    return QString::fromUtf8(doc.toJson(QJsonDocument::Indented));

}
