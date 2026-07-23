#include "view_layer/splitpane.h"
#include <QEvent>
#include <QPainter>
#include <QPen>
#include <QTimer>
#include <QVBoxLayout>
#include "shared/dto_bidirectional/editorsettingsdto.h"
#include "theme.h"
#include "view_layer/customscrollbar.h"
#include <shared/dto_model_to_view/gridviewstatedto.h>
#include <view_layer/editor/editor.h>
#include <view_layer/editor/editorcontainer.h>

// SplitPaneHandle inner class:

SplitPaneHandle::SplitPaneHandle(int topPadding, Qt::Orientation orientation, QSplitter* parent)
    : QSplitterHandle(orientation, parent), m_topPadding(topPadding), m_hovered(false) {
    setCursor(Qt::SizeHorCursor);
}

void SplitPaneHandle::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    // painter.fillRect(rect(), Theme::almostBlack());

    // solid line on the right edge
    QPen solidPen(Theme::darkerGray(), 1);
    painter.setPen(solidPen);
    painter.drawLine(width() - 1, m_topPadding, width() - 1, height());

    // dotted line in the middle
    QPen dottedPen(Theme::almostWhite(), 1, Qt::DashLine);
    painter.setPen(dottedPen);
    int lineX = 3;
    if (m_hovered)
    painter.drawLine(width() - 1, m_topPadding, width() - 1, height());

}

void SplitPaneHandle::enterEvent(QEnterEvent* event) {
    m_hovered = true;
    update();
}

void SplitPaneHandle::leaveEvent(QEvent* event) {
    m_hovered = false;
    update();
}

// SplitPane class

SplitPane::SplitPane(int leftWidth, int separatorTopPadding, QWidget* parent)
    : QSplitter(Qt::Horizontal, parent), m_separatorTopPadding(separatorTopPadding) {
    setHandleWidth(7);
    setChildrenCollapsible(false);

    m_leftPane  = new QWidget(this);
    m_rightPane = new QWidget(this);
    m_leftPane->setMinimumWidth(100);
    m_rightPane->setMinimumWidth(100);

    addWidget(m_leftPane);
    addWidget(m_rightPane);

    setStretchFactor(0, 0); // no stretch
    setStretchFactor(1, 1); // takes all extra space

    setSizes({leftWidth, 9999});

    // scrollarea:

    m_scrollArea = new QScrollArea(m_leftPane);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    m_scrollArea->setStyleSheet(m_scrollArea->styleSheet() +
                                "QScrollArea { background: transparent; border: none; }"
                                );
    m_scrollArea->viewport()->setStyleSheet("background: transparent;");

    // Editor:
    EditorSettingsDTO editorSettingsDTO{20, 0.5, false};
    m_editor = new Editor(editorSettingsDTO, this);

    m_scrollArea->setWidgetResizable(false); // must be false - this is the default, just be explicit
    m_scrollArea->setWidget(m_editor);

    m_editor->show();
    m_editor->updateWidth(200);
    m_editor->updateHeight(2000);

    QVBoxLayout* leftLayout = new QVBoxLayout(m_leftPane);
    leftLayout->setContentsMargins(0, 40, 0, 0);
    leftLayout->setSpacing(0);
    leftLayout->addWidget(m_scrollArea);
    m_leftPane->setLayout(leftLayout);

    // Grid:
    GridViewStateDTO gridViewStateDTO{25, 30.0, QPoint(0, 0), {}};
    m_grid = new Grid(gridViewStateDTO, m_rightPane);

    QVBoxLayout *rightLayout = new QVBoxLayout(m_rightPane);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);
    rightLayout->addWidget(m_grid);
    m_rightPane->setLayout(rightLayout);

    // scrollbars:

    m_verticalScrollBar = new CustomScrollBar(Qt::Vertical, m_scrollArea);
    m_horizontalScrollBar = new CustomScrollBar(Qt::Horizontal, m_scrollArea);

    m_scrollArea->setVerticalScrollBar(m_verticalScrollBar);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setHorizontalScrollBar(m_horizontalScrollBar);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    m_verticalScrollBar->hide();
    m_horizontalScrollBar->hide();

    m_scrollArea->installEventFilter(this);
    m_editor->installEventFilter(this);
    m_verticalScrollBar->installEventFilter(this);
    m_horizontalScrollBar->installEventFilter(this);

    QString scrollBarStyle =
        "QScrollBar:vertical { background: transparent; width: 6px; }"
        "QScrollBar::handle:vertical { background: transparent; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: transparent; }"
        "QScrollBar:horizontal { background: transparent; height: 11px; }"
        "QScrollBar::handle:horizontal { background: transparent; }"
        "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0px; }"
        "QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal { background: transparent; }"
        ;

    m_verticalScrollBar->setStyleSheet(scrollBarStyle);
    m_horizontalScrollBar->setStyleSheet(scrollBarStyle);

    // timer to hide scrollbars reliably of not hovered:
    m_scrollBarHideTimer = new QTimer(this);
    m_scrollBarHideTimer->setSingleShot(true);
    m_scrollBarHideTimer->setInterval(100);
    connect(m_scrollBarHideTimer, &QTimer::timeout, this, [this]() {
            m_verticalScrollBar->hide();
            m_horizontalScrollBar->hide();
    });

    setAttribute(Qt::WA_StaticContents); // telling Qt that the content doesn't change during resize, this can help with redraw perf.

    connect(m_editor, &Editor::editorStateChanged, this, &SplitPane::editorStateChanged);
    connect(m_editor, &Editor::editorCursorPosChanged, this, &SplitPane::editorCursorPosChanged);
    connect(m_editor, &Editor::editorKeyPressed, this, &SplitPane::editorKeyPressed);
    connect(m_grid, &Grid::gridZoomChanged, this, &SplitPane::gridZoomChanged);
    connect(m_grid, &Grid::gridDragged, this, &SplitPane::gridDragged);
    connect(m_grid, &Grid::boxDragged, this, &SplitPane::boxDragged);
}

QSplitterHandle* SplitPane::createHandle() {
    return new SplitPaneHandle(m_separatorTopPadding, orientation(), this);
}

QWidget* SplitPane::leftPane() const {
    return m_leftPane;
}

QWidget* SplitPane::rightPane() const {
    return m_rightPane;
}

void SplitPane::updateEditorState(const EditorViewStateDTO &dto)
{
    m_editor->updateEditorState(dto);
}

void SplitPane::updateEditorCursorPos(const EditorCursorPosDTO &dto)
{
    m_editor->updateCursorPosition(dto);
}

void SplitPane::updateEditorSettings(const EditorSettingsDTO &dto)
{
    m_editor->setSettings(dto);
}

void SplitPane::updateGridViewState(const GridViewStateDTO &dto)
{
    m_grid->updateGridViewState(dto);
}

bool SplitPane::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::Enter) {
        m_scrollBarHideTimer->stop();
        m_verticalScrollBar->show();
        m_horizontalScrollBar->show();
    } else if (event->type() == QEvent::Leave) {
        m_scrollBarHideTimer->start();
    }
    return QSplitter::eventFilter(obj, event);
}
