#ifndef HSWIDGETCOMPONENTPARSER_H
#define HSWIDGETCOMPONENTPARSER_H

#include <QObject>
#include <QXmlStreamReader>
#include "hswidgetcomponentdescriptor.h"

class HsWidgetComponentParser : public QObject
{
    Q_OBJECT
    
public:
    HsWidgetComponentParser(const QString &aComponentManifest, QObject *parent=0);
    ~HsWidgetComponentParser();
    HsWidgetComponentDescriptor widgetComponentDescriptor();
    bool error();
private:
    void parseFile(const QString &aComponentManifest);
    void parseWidgetManifestTag();
    void parseWidgetProviderTag();
    
    void parseVersion1_0();

    void parseUri();
    void parseIcon();
    void parseTitle();
    void parseDescription();
    void parseHidden();
    void parseServiceXml();
    
   
    bool isWidgetTagValid();
private:
    QXmlStreamReader mParser;
    HsWidgetComponentDescriptor mComponentDescriptor;
    bool mError;
};

#endif // HSWIDGETCOMPONENTPARSER_H
