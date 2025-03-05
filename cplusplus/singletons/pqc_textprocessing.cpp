#include <pqc_textprocessing.h>

PQCTextProcessing::PQCTextProcessing() {

}

PQCTextProcessing::~PQCTextProcessing() {

}

QString PQCTextProcessing::prettifyJSON(QString json) {

    QString pretty = json;

    while(pretty.contains("  ")) {
        pretty = pretty.replace("  "," ");
    }
    pretty = pretty.replace("\n","");

    // add necessary line breaks
    pretty = pretty.replace("},","}COMMA");
    const QStringList linebreaksAfter = {",", "[", "]", "{", "}"};
    for(auto &l : linebreaksAfter)
        pretty = pretty.replace(l, l+"\n");
    const QStringList linebreaksBefore = {"]", "{", "}"};
    for(auto &l : linebreaksBefore)
        pretty = pretty.replace(l, "\n"+l);
    pretty = pretty.replace("}\nCOMMA", "},");

    // go through json line by line
    QStringList parts = pretty.split("\n");

    // this will hold the prettified string
    pretty = "";

    // how much indentation we have
    int depth = -1;

    for(int i = 0; i < parts.length(); ++i) {

        QString l = parts[i].trimmed();

        // ignore empty lines
        if(l == "")
            continue;

        // starting a block increases depth by one
        if(l == "{")
            depth += 1;

        // ending a block or list devreases depth by one
        if(l == "]" || l == "}" || l == "},")
            depth -= 1;

        pretty += l.rightJustified(l.length()+4*depth, ' ') + "\n";

        // indent everything inside a list/block
        if(l.endsWith("[") || l == "{")
            depth += 1;

        // done with list/block
        if(l == "]" || l == "}" || l == "},")
            depth -= 1;

    }

    return pretty;
}
