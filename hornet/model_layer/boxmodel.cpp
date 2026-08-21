#include "boxmodel.h"
#include <algorithm>
BoxModel::BoxModel(int id,
                   int posX,
                   int posY,
                   int width,
                   int height,
                   const QString &headerText,
                   const QVector<QString> &bodyLines,
                   bool isFileBacked,
                   const QString &originFilePath)
    : m_id(id)
    , m_posX(posX)
    , m_posY(posY)
    , m_width(std::max(width, m_minWidth))
    , m_height(std::max(height, m_minHeight))
    , m_headerText(headerText)
    , m_bodyLines(bodyLines)
    , m_contentType(determineContentTypeFromHeaderText(headerText))
    , m_isFileBacked(isFileBacked)
    , m_originFilePath(originFilePath)
{}
int BoxModel::getId() const
{
    return m_id;
}
int BoxModel::getPosX() const
{
    return m_posX;
}
int BoxModel::getPosY() const
{
    return m_posY;
}
int BoxModel::getWidth() const
{
    return m_width;
}
int BoxModel::getHeight() const
{
    return m_height;
}
QString BoxModel::getHeaderText() const
{
    return m_headerText;
}
QVector<QString> BoxModel::getBodyLines() const
{
    return m_bodyLines;
}
BoxContentType BoxModel::getContentType() const
{
    return m_contentType;
}
void BoxModel::setPosX(int posX)
{
    m_posX = posX;
}
void BoxModel::setPosY(int posY)
{
    m_posY = posY;
}
void BoxModel::setWidth(int width)
{
    m_width = std::max(width, m_minWidth);
}
void BoxModel::setHeight(int height)
{
    m_height = std::max(height, m_minHeight);
}
void BoxModel::setHeaderText(const QString &headerText)
{
    m_headerText = headerText;
    m_contentType = determineContentTypeFromHeaderText(headerText);
}
void BoxModel::setBodyLines(const QVector<QString> &bodyLines)
{
    m_bodyLines = bodyLines;
}
BoxContentType BoxModel::determineContentTypeFromHeaderText(const QString &headerText)
{
    if (headerText == "hornet.log")
        return BoxContentType::PlainText;
    if (headerText.endsWith(".txt"))
        return BoxContentType::PlainText;
    if (headerText.endsWith(".terminal"))
        return BoxContentType::Terminal;
    if (headerText.endsWith(".tool"))
        return BoxContentType::Tool;
    if (headerText.endsWith(".list"))
        return BoxContentType::List;
    return BoxContentType::Unknown;
}

int BoxModel::getCursorX() const
{
    return m_cursorX;
}
int BoxModel::getCursorY() const
{
    return m_cursorY;
}
void BoxModel::setCursorPos(int cursorX, int cursorY)
{
    m_cursorX = cursorX;
    m_cursorY = cursorY;
}

int BoxModel::getBodyScrollOffset() const
{
    return m_bodyScrollOffset;
}
void BoxModel::setBodyScrollOffset(int offset)
{
    m_bodyScrollOffset = std::max(0, offset);
}

int BoxModel::getSelectionAnchorX() const
{
    return m_selectionAnchorX;
}

int BoxModel::getSelectionAnchorY() const
{
    return m_selectionAnchorY;
}

int BoxModel::getSelectionExtentX() const
{
    return m_selectionExtentX;
}

int BoxModel::getSelectionExtentY() const
{
    return m_selectionExtentY;
}

bool BoxModel::hasSelection() const
{
    return m_hasSelection;
}

void BoxModel::setSelection(int anchorX, int anchorY, int extentX, int extentY, bool hasSelection)
{
    m_selectionAnchorX = anchorX;
    m_selectionAnchorY = anchorY;
    m_selectionExtentX = extentX;
    m_selectionExtentY = extentY;
    m_hasSelection = hasSelection;
}

bool BoxModel::isFileBacked() const
{
    return m_isFileBacked;
}
QString BoxModel::getOriginFilePath() const
{
    return m_originFilePath;
}
QString BoxModel::getToolFieldValue(const QString &name) const
{
    return m_toolFieldValues.value(name);
}
void BoxModel::setToolFieldValue(const QString &name, const QString &value)
{
    m_toolFieldValues[name] = value;
}
QHash<QString, QString> BoxModel::getToolFieldValues() const
{
    return m_toolFieldValues;
}

QVector<MarkRange> BoxModel::getMarks() const
{
    return m_marks;
}
void BoxModel::setMarks(const QVector<MarkRange> &marks)
{
    m_marks = marks;
}
