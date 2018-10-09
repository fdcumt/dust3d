#include <map>
#include <QMutex>
#include <QMutexLocker>
#include "imageforever.h"

struct ImageForeverItem
{
    QImage *image;
    QUuid id;
};
static std::map<QUuid, ImageForeverItem> g_foreverMap;
static std::map<qint64, QUuid> g_foreverCacheKeyToIdMap;
static QMutex g_mapMutex;

const QImage *ImageForever::get(const QUuid &id)
{
    QMutexLocker locker(&g_mapMutex);
    auto findResult = g_foreverMap.find(id);
    if (findResult == g_foreverMap.end())
        return nullptr;
    return findResult->second.image;
}

QUuid ImageForever::add(const QImage *image, QUuid toId)
{
    QMutexLocker locker(&g_mapMutex);
    if (nullptr == image)
        return QUuid();
    auto key = image->cacheKey();
    auto findResult = g_foreverCacheKeyToIdMap.find(key);
    if (findResult != g_foreverCacheKeyToIdMap.end()) {
        return findResult->second;
    }
    QUuid newId = toId.isNull() ? QUuid::createUuid() : toId;
    if (g_foreverMap.find(newId) != g_foreverMap.end())
        return newId;
    QImage *newImage = new QImage(*image);
    g_foreverMap[newId] = {newImage, newId};
    g_foreverCacheKeyToIdMap[newImage->cacheKey()] = newId;
    return newId;
}
