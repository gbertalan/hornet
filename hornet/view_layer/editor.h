#pragma once

#include "font_renderer/GlyphAtlas.h"
#include "font_renderer/CpuGlyphRenderer.h"
#include <QWidget>
#include <memory>

class Editor : public QWidget
{
    Q_OBJECT
public:
    explicit Editor(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    GlyphAtlas m_atlas;
    std::unique_ptr<CpuGlyphRenderer> m_renderer;
};
