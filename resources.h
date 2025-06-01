#ifndef RESOURCES_H
#define RESOURCES_H

#include <QString>
#include <QMap>

class Resources {
public:
    Resources(const QString &filePath);

    QMap<QString, int> resourceCount;

private:
    void loadResources(const QString &filePath);
};

#endif // RESOURCES_H
