#include <QPainter>
#include <QGuiApplication>
#include <QRegion>
#include <QPolygon>
#include "texturegenerator.h"
#include "theme.h"

int TextureGenerator::m_textureSize = 1024;

TextureGenerator::TextureGenerator(const MeshResultContext &meshResultContext) :
    m_resultTextureGuideImage(nullptr),
    m_resultTextureImage(nullptr),
    m_resultTextureBorderImage(nullptr),
    m_resultTextureColorImage(nullptr),
    m_resultTextureNormalImage(nullptr),
    m_resultTextureMetalnessRoughnessAmbientOcclusionImage(nullptr),
    m_resultTextureRoughnessImage(nullptr),
    m_resultTextureMetalnessImage(nullptr),
    m_resultTextureAmbientOcclusionImage(nullptr),
    m_resultMesh(nullptr)
{
    m_resultContext = new MeshResultContext();
    *m_resultContext = meshResultContext;
}

TextureGenerator::~TextureGenerator()
{
    delete m_resultContext;
    delete m_resultTextureGuideImage;
    delete m_resultTextureImage;
    delete m_resultTextureBorderImage;
    delete m_resultTextureColorImage;
    delete m_resultTextureNormalImage;
    delete m_resultTextureMetalnessRoughnessAmbientOcclusionImage;
    delete m_resultTextureRoughnessImage;
    delete m_resultTextureMetalnessImage;
    delete m_resultTextureAmbientOcclusionImage;
    delete m_resultMesh;
}

QImage *TextureGenerator::takeResultTextureGuideImage()
{
    QImage *resultTextureGuideImage = m_resultTextureGuideImage;
    m_resultTextureGuideImage = nullptr;
    return resultTextureGuideImage;
}

QImage *TextureGenerator::takeResultTextureImage()
{
    QImage *resultTextureImage = m_resultTextureImage;
    m_resultTextureImage = nullptr;
    return resultTextureImage;
}

QImage *TextureGenerator::takeResultTextureBorderImage()
{
    QImage *resultTextureBorderImage = m_resultTextureBorderImage;
    m_resultTextureBorderImage = nullptr;
    return resultTextureBorderImage;
}

QImage *TextureGenerator::takeResultTextureColorImage()
{
    QImage *resultTextureColorImage = m_resultTextureColorImage;
    m_resultTextureColorImage = nullptr;
    return resultTextureColorImage;
}

QImage *TextureGenerator::takeResultTextureNormalImage()
{
    QImage *resultTextureNormalImage = m_resultTextureNormalImage;
    m_resultTextureNormalImage = nullptr;
    return resultTextureNormalImage;
}

MeshResultContext *TextureGenerator::takeResultContext()
{
    MeshResultContext *resultContext = m_resultContext;
    m_resultTextureImage = nullptr;
    return resultContext;
}

MeshLoader *TextureGenerator::takeResultMesh()
{
    MeshLoader *resultMesh = m_resultMesh;
    m_resultMesh = nullptr;
    return resultMesh;
}

void TextureGenerator::addPartColorMap(QUuid partId, const QImage *image)
{
    if (nullptr == image)
        return;
    m_partColorTextureMap[partId] = *image;
}

void TextureGenerator::addPartNormalMap(QUuid partId, const QImage *image)
{
    if (nullptr == image)
        return;
    m_partNormalTextureMap[partId] = *image;
}

void TextureGenerator::addPartMetalnessMap(QUuid partId, const QImage *image)
{
    if (nullptr == image)
        return;
    m_partMetalnessTextureMap[partId] = *image;
}

void TextureGenerator::addPartRoughnessMap(QUuid partId, const QImage *image)
{
    if (nullptr == image)
        return;
    m_partRoughnessTextureMap[partId] = *image;
}

void TextureGenerator::addPartAmbientOcclusionMap(QUuid partId, const QImage *image)
{
    if (nullptr == image)
        return;
    m_partAmbientOcclusionTextureMap[partId] = *image;
}

QPainterPath TextureGenerator::expandedPainterPath(const QPainterPath &painterPath)
{
    QPainterPathStroker stroker;
    stroker.setWidth(20);
    stroker.setJoinStyle(Qt::MiterJoin);
    return (stroker.createStroke(painterPath) + painterPath).simplified();
}

void TextureGenerator::generate()
{
    bool hasNormalMap = false;
    bool hasMetalnessMap = false;
    bool hasRoughnessMap = false;
    bool hasAmbientOcclusionMap = false;
    
    const std::vector<Material> &triangleMaterials = m_resultContext->triangleMaterials();
    const std::vector<ResultTriangleUv> &triangleUvs = m_resultContext->triangleUvs();
    
    m_resultTextureColorImage = new QImage(TextureGenerator::m_textureSize, TextureGenerator::m_textureSize, QImage::Format_ARGB32);
    m_resultTextureColorImage->fill(Theme::white);
    
    m_resultTextureBorderImage = new QImage(TextureGenerator::m_textureSize, TextureGenerator::m_textureSize, QImage::Format_ARGB32);
    m_resultTextureBorderImage->fill(Qt::transparent);
    
    m_resultTextureNormalImage = new QImage(TextureGenerator::m_textureSize, TextureGenerator::m_textureSize, QImage::Format_ARGB32);
    m_resultTextureNormalImage->fill(Qt::transparent);
    
    m_resultTextureMetalnessRoughnessAmbientOcclusionImage = new QImage(TextureGenerator::m_textureSize, TextureGenerator::m_textureSize, QImage::Format_ARGB32);
    m_resultTextureMetalnessRoughnessAmbientOcclusionImage->fill(Qt::transparent);
    
    m_resultTextureMetalnessImage = new QImage(TextureGenerator::m_textureSize, TextureGenerator::m_textureSize, QImage::Format_ARGB32);
    m_resultTextureMetalnessImage->fill(Qt::transparent);
    
    m_resultTextureRoughnessImage = new QImage(TextureGenerator::m_textureSize, TextureGenerator::m_textureSize, QImage::Format_ARGB32);
    m_resultTextureRoughnessImage->fill(Qt::transparent);
    
    m_resultTextureAmbientOcclusionImage = new QImage(TextureGenerator::m_textureSize, TextureGenerator::m_textureSize, QImage::Format_ARGB32);
    m_resultTextureAmbientOcclusionImage->fill(Qt::transparent);
    
    QColor borderColor = Qt::darkGray;
    QPen pen(borderColor);
    
    QPainter texturePainter;
    texturePainter.begin(m_resultTextureColorImage);
    texturePainter.setRenderHint(QPainter::Antialiasing);
    texturePainter.setRenderHint(QPainter::HighQualityAntialiasing);
    
    QPainter textureBorderPainter;
    textureBorderPainter.begin(m_resultTextureBorderImage);
    textureBorderPainter.setRenderHint(QPainter::Antialiasing);
    textureBorderPainter.setRenderHint(QPainter::HighQualityAntialiasing);
    
    QPainter textureNormalPainter;
    textureNormalPainter.begin(m_resultTextureNormalImage);
    textureNormalPainter.setRenderHint(QPainter::Antialiasing);
    textureNormalPainter.setRenderHint(QPainter::HighQualityAntialiasing);
    
    QPainter textureMetalnessPainter;
    textureMetalnessPainter.begin(m_resultTextureMetalnessImage);
    textureMetalnessPainter.setRenderHint(QPainter::Antialiasing);
    textureMetalnessPainter.setRenderHint(QPainter::HighQualityAntialiasing);
    
    QPainter textureRoughnessPainter;
    textureRoughnessPainter.begin(m_resultTextureRoughnessImage);
    textureRoughnessPainter.setRenderHint(QPainter::Antialiasing);
    textureRoughnessPainter.setRenderHint(QPainter::HighQualityAntialiasing);
    
    QPainter textureAmbientOcclusionPainter;
    textureAmbientOcclusionPainter.begin(m_resultTextureAmbientOcclusionImage);
    textureAmbientOcclusionPainter.setRenderHint(QPainter::Antialiasing);
    textureAmbientOcclusionPainter.setRenderHint(QPainter::HighQualityAntialiasing);
 
    // round 1, paint background
    for (auto i = 0u; i < triangleUvs.size(); i++) {
        QPainterPath path;
        const ResultTriangleUv *uv = &triangleUvs[i];
        for (auto j = 0; j < 3; j++) {
            if (0 == j) {
                path.moveTo(uv->uv[j][0] * TextureGenerator::m_textureSize, uv->uv[j][1] * TextureGenerator::m_textureSize);
            } else {
                path.lineTo(uv->uv[j][0] * TextureGenerator::m_textureSize, uv->uv[j][1] * TextureGenerator::m_textureSize);
            }
        }
        path = expandedPainterPath(path);
        QPen textureBorderPen(triangleMaterials[i].color);
        textureBorderPen.setWidth(32);
        texturePainter.setPen(textureBorderPen);
        texturePainter.setBrush(QBrush(triangleMaterials[i].color));
        texturePainter.drawPath(path);
    }
    // round 2, real paint
    texturePainter.setPen(Qt::NoPen);
    for (auto i = 0u; i < triangleUvs.size(); i++) {
        QPainterPath path;
        const ResultTriangleUv *uv = &triangleUvs[i];
        float points[][2] = {
            {uv->uv[0][0] * TextureGenerator::m_textureSize, uv->uv[0][1] * TextureGenerator::m_textureSize},
            {uv->uv[1][0] * TextureGenerator::m_textureSize, uv->uv[1][1] * TextureGenerator::m_textureSize},
            {uv->uv[2][0] * TextureGenerator::m_textureSize, uv->uv[2][1] * TextureGenerator::m_textureSize}
        };
        path.moveTo(points[0][0], points[0][1]);
        path.lineTo(points[1][0], points[1][1]);
        path.lineTo(points[2][0], points[2][1]);
        path = expandedPainterPath(path);
        // Fill base color
        texturePainter.fillPath(path, QBrush(triangleMaterials[i].color));
        // Copy color texture if there is one
        const std::pair<QUuid, QUuid> source = m_resultContext->triangleSourceNodes()[i];
        auto findColorTextureResult = m_partColorTextureMap.find(source.first);
        if (findColorTextureResult != m_partColorTextureMap.end()) {
            texturePainter.setClipping(true);
            texturePainter.setClipPath(path);
            QPen textureBorderPen(triangleMaterials[i].color);
            textureBorderPen.setWidth(0);
            texturePainter.setPen(textureBorderPen);
            texturePainter.drawImage(0, 0, findColorTextureResult->second);
            texturePainter.setPen(Qt::NoPen);
            texturePainter.setClipping(false);
        }
        // Copy normal texture if there is one
        auto findNormalTextureResult = m_partNormalTextureMap.find(source.first);
        if (findNormalTextureResult != m_partNormalTextureMap.end()) {
            textureNormalPainter.setClipping(true);
            textureNormalPainter.setClipPath(path);
            QPen textureBorderPen(triangleMaterials[i].color);
            textureBorderPen.setWidth(0);
            textureNormalPainter.setPen(textureBorderPen);
            textureNormalPainter.drawImage(0, 0, findNormalTextureResult->second);
            textureNormalPainter.setPen(Qt::NoPen);
            textureNormalPainter.setClipping(false);
            hasNormalMap = true;
        }
        // Copy metalness texture if there is one
        auto findMetalnessTextureResult = m_partMetalnessTextureMap.find(source.first);
        if (findMetalnessTextureResult != m_partMetalnessTextureMap.end()) {
            textureMetalnessPainter.setClipping(true);
            textureMetalnessPainter.setClipPath(path);
            QPen textureBorderPen(triangleMaterials[i].color);
            textureBorderPen.setWidth(0);
            textureMetalnessPainter.setPen(textureBorderPen);
            textureMetalnessPainter.drawImage(0, 0, findMetalnessTextureResult->second);
            textureMetalnessPainter.setPen(Qt::NoPen);
            textureMetalnessPainter.setClipping(false);
            hasMetalnessMap = true;
        }
        // Copy roughness texture if there is one
        auto findRoughnessTextureResult = m_partRoughnessTextureMap.find(source.first);
        if (findRoughnessTextureResult != m_partRoughnessTextureMap.end()) {
            textureRoughnessPainter.setClipping(true);
            textureRoughnessPainter.setClipPath(path);
            QPen textureBorderPen(triangleMaterials[i].color);
            textureBorderPen.setWidth(0);
            textureRoughnessPainter.setPen(textureBorderPen);
            textureRoughnessPainter.drawImage(0, 0, findRoughnessTextureResult->second);
            textureRoughnessPainter.setPen(Qt::NoPen);
            textureRoughnessPainter.setClipping(false);
            hasRoughnessMap = true;
        }
        // Copy ambient occlusion texture if there is one
        auto findAmbientOcclusionTextureResult = m_partAmbientOcclusionTextureMap.find(source.first);
        if (findAmbientOcclusionTextureResult != m_partAmbientOcclusionTextureMap.end()) {
            textureAmbientOcclusionPainter.setClipping(true);
            textureAmbientOcclusionPainter.setClipPath(path);
            QPen textureBorderPen(triangleMaterials[i].color);
            textureBorderPen.setWidth(0);
            textureAmbientOcclusionPainter.setPen(textureBorderPen);
            textureAmbientOcclusionPainter.drawImage(0, 0, findAmbientOcclusionTextureResult->second);
            textureAmbientOcclusionPainter.setPen(Qt::NoPen);
            textureAmbientOcclusionPainter.setClipping(false);
            hasAmbientOcclusionMap = true;
        }
    }
    
    pen.setWidth(0);
    textureBorderPainter.setPen(pen);
    for (auto i = 0u; i < triangleUvs.size(); i++) {
        const ResultTriangleUv *uv = &triangleUvs[i];
        for (auto j = 0; j < 3; j++) {
            int from = j;
            int to = (j + 1) % 3;
            textureBorderPainter.drawLine(uv->uv[from][0] * TextureGenerator::m_textureSize, uv->uv[from][1] * TextureGenerator::m_textureSize,
                uv->uv[to][0] * TextureGenerator::m_textureSize, uv->uv[to][1] * TextureGenerator::m_textureSize);
        }
    }
    
    texturePainter.end();
    textureBorderPainter.end();
    textureNormalPainter.end();
    textureMetalnessPainter.end();
    textureRoughnessPainter.end();
    textureAmbientOcclusionPainter.end();
    
    if (!hasNormalMap) {
        delete m_resultTextureNormalImage;
        m_resultTextureNormalImage = nullptr;
    }
    
    if (!hasMetalnessMap && !hasRoughnessMap && !hasAmbientOcclusionMap) {
        delete m_resultTextureMetalnessRoughnessAmbientOcclusionImage;
        m_resultTextureMetalnessRoughnessAmbientOcclusionImage = nullptr;
    } else {
        for (int row = 0; row < m_resultTextureMetalnessRoughnessAmbientOcclusionImage->height(); ++row) {
            for (int col = 0; col < m_resultTextureMetalnessRoughnessAmbientOcclusionImage->width(); ++col) {
                QColor color;
                if (hasMetalnessMap)
                    color.setBlue(qGray(m_resultTextureMetalnessImage->pixel(col, row)));
                if (hasRoughnessMap)
                    color.setRed(qGray(m_resultTextureRoughnessImage->pixel(col, row)));
                if (hasAmbientOcclusionMap)
                    color.setGreen(qGray(m_resultTextureAmbientOcclusionImage->pixel(col, row)));
                m_resultTextureMetalnessRoughnessAmbientOcclusionImage->setPixelColor(col, row, color);
            }
        }
    }
    
    m_resultTextureImage = new QImage(*m_resultTextureColorImage);
    
    m_resultTextureGuideImage = new QImage(*m_resultTextureImage);
    QPainter mergeTextureGuidePainter(m_resultTextureGuideImage);
    mergeTextureGuidePainter.setCompositionMode(QPainter::CompositionMode_Multiply);
    mergeTextureGuidePainter.drawImage(0, 0, *m_resultTextureBorderImage);
    mergeTextureGuidePainter.end();
    
    m_resultMesh = new MeshLoader(*m_resultContext);
    m_resultMesh->setTextureImage(new QImage(*m_resultTextureImage));
    if (nullptr != m_resultTextureNormalImage)
        m_resultMesh->setNormalMapImage(new QImage(*m_resultTextureNormalImage));
    if (nullptr != m_resultTextureMetalnessRoughnessAmbientOcclusionImage) {
        m_resultMesh->setMetalnessRoughnessAmbientOcclusionImage(new QImage(*m_resultTextureMetalnessRoughnessAmbientOcclusionImage));
        m_resultMesh->setHasMetalnessInImage(hasMetalnessMap);
        m_resultMesh->setHasRoughnessInImage(hasRoughnessMap);
        m_resultMesh->setHasAmbientOcclusionInImage(hasAmbientOcclusionMap);
    }
}

void TextureGenerator::process()
{
    generate();

    this->moveToThread(QGuiApplication::instance()->thread());
    emit finished();
}
