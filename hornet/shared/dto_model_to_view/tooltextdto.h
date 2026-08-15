#ifndef RENDERTEXTDTO_H
#define RENDERTEXTDTO_H
#include <QString>
struct ToolTextDTO
{
    double x;
    double y;
    QString text;
    QString colorToken;
    double fontSizeMultiplier;
    ToolTextDTO(double x,
                double y,
                const QString &text,
                const QString &colorToken = QString(),
                double fontSizeMultiplier = 1.0)
        : x(x)
        , y(y)
        , text(text)
        , colorToken(colorToken)
        , fontSizeMultiplier(fontSizeMultiplier)
    {}
};
#endif // RENDERTEXTDTO_H