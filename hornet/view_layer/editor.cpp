#include "editor.h"
#include "theme.h"
#include <QPainter>

Editor::Editor(QWidget* parent) : QWidget(parent)
{
    m_atlas.addFont(":/fonts/JetBrainsMono-Bold.ttf"); // primary
    m_atlas.addFont(":/fonts/NotoSansMono-Bold.ttf"); // fallback (secondary)
    m_atlas.addFont(":/fonts/NotoSansCJK-Regular.ttc"); // fallback (cjk)

    m_renderer = std::make_unique<CpuGlyphRenderer>(m_atlas);
}

void Editor::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    m_renderer->drawText(painter, 20.f, 30.f, "Hello!", Qt::white, 0.7f);
    m_renderer->drawText(painter, 140.f, 70.f, "mov rax, rbx", Theme::brightAmber(), 2.0f);
    m_renderer->drawText(painter, 140.f, 130.f, "0xDEADBEEF", Qt::cyan, 0.86f);
}
