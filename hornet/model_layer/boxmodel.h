#ifndef BOXMODEL_H
#define BOXMODEL_H
#include <QString>
#include <QVector>
#include "model_layer/boxcontenttype.h"
class BoxModel
{
public:
    explicit BoxModel(int id,
                      int posX,
                      int posY,
                      int width,
                      int height,
                      const QString &headerText,
                      const QVector<QString> &bodyLines);
    int getId() const;
    int getPosX() const;
    int getPosY() const;
    int getWidth() const;
    int getHeight() const;
    QString getHeaderText() const;
    QVector<QString> getBodyLines() const;
    BoxContentType getContentType() const;
    void setPosX(int posX);
    void setPosY(int posY);
    void setWidth(int width);
    void setHeight(int height);
    void setHeaderText(const QString &headerText);
    void setBodyLines(const QVector<QString> &bodyLines);

private:
    int m_id;
    int m_posX;
    int m_posY;
    int m_width;
    int m_height;
    QString m_headerText;
    QVector<QString> m_bodyLines;
    BoxContentType m_contentType;
    static constexpr int m_minWidth = 5;
    static constexpr int m_minHeight = 3;
    static BoxContentType determineContentTypeFromHeaderText(const QString &headerText);
};
#endif // BOXMODEL_H
