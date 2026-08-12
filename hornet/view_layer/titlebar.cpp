#include "view_layer/titlebar.h"
#include "view_layer/titlebarbutton.h"
#include "view_layer/titlebarfilenamebutton.h"

#include "theme.h"
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPalette>
#include <QScreen>
#include <QTimer>

TitleBar::TitleBar(FontAtlas &fontAtlas, FontRenderer &fontRenderer, QWidget *parent)
    : QWidget(parent)
    , m_dragging(false)
    , m_doubleclicked(false)
    , m_grabRatio(0.0)
{
    m_window = qobject_cast<Window *>(parent);
    setFixedHeight(40);
    setAttribute(Qt::WA_TranslucentBackground);

    m_fileNameButton = new TitlebarFileNameButton(fontAtlas, fontRenderer, this);
    m_fileNameButton->setFixedSize(220, 38);

    connect(m_fileNameButton,
            &TitlebarFileNameButton::clicked,
            this,
            &TitleBar::fileNameButtonClicked);

    m_trayButton = new TitlebarButton(TitlebarButtonType::Tray, this);
    m_maxMinButton = new TitlebarButton(TitlebarButtonType::Maximize, this);
    m_closeButton = new TitlebarButton(TitlebarButtonType::Close, this);
    m_closeButton->setHoverColor(Theme::mediumRed());
    m_closeButton->setRightPadding(5);

    m_fileLoadButton = new TitlebarButton(TitlebarButtonType::FileLoad, this);
    m_projectSaveButton = new TitlebarButton(TitlebarButtonType::ProjectSave, this);
    m_scriptRunButton = new TitlebarButton(TitlebarButtonType::ScriptRun, this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignTop);

    layout->addWidget(m_fileLoadButton);
    layout->addWidget(m_projectSaveButton);
    layout->addWidget(m_scriptRunButton);

    layout->addStretch();

    layout->addWidget(m_trayButton);
    layout->addWidget(m_maxMinButton);
    layout->addWidget(m_closeButton);

    layout->setAlignment(m_closeButton, Qt::AlignTop);
    setLayout(layout);

    connect(m_fileLoadButton, &TitlebarButton::clicked, this, &TitleBar::fileLoadButtonClicked);
    connect(m_projectSaveButton,
            &TitlebarButton::clicked,
            this,
            &TitleBar::projectSaveButtonClicked);
    connect(m_scriptRunButton, &TitlebarButton::clicked, this, &TitleBar::scriptRunButtonClicked);

    connect(m_trayButton, &TitlebarButton::clicked, this, &TitleBar::minimizeClicked);
    connect(m_maxMinButton, &TitlebarButton::clicked, this, &TitleBar::maximizeClicked);
    connect(m_closeButton, &TitlebarButton::clicked, this, &TitleBar::windowCloseClicked);
}

void TitleBar::setFullscreen(bool fullscreen) {
    m_maxMinButton->setType(fullscreen ? TitlebarButtonType::Minimize : TitlebarButtonType::Maximize);
}

void TitleBar::updateFileName(const QString &fileName)
{
    m_currentFileName = fileName;
    m_fileNameButton->setFileName(fileName);
}

void TitleBar::updateFileNameButtonPosition(int leftPaneWidth)
{
    int centerX = leftPaneWidth / 2;
    int x = std::max(0, centerX - m_fileNameButton->width() / 2);
    m_fileNameButton->setGeometry(x, 0, m_fileNameButton->width(), m_fileNameButton->height());
}

QString TitleBar::currentFileName() const
{
    return m_currentFileName;
}

QPoint TitleBar::fileNameDropdownAnchor() const
{
    return m_fileNameButton->mapTo(parentWidget(), QPoint(0, 0));
}

void TitleBar::updateCurrentBoxId(int boxId)
{
    m_currentBoxId = boxId;
}

void TitleBar::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.fillRect(rect(), Theme::darkGrayTranslucent());
    painter.setPen(QPen(QColor(10, 10, 10, 230), 2));
    painter.drawLine(0, height() - 1, width(), height() - 1);
}

void TitleBar::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_grabRatio = static_cast<double>(event->pos().x()) / width();
        m_dragStartPosition = event->globalPosition().toPoint() - window()->frameGeometry().topLeft();
    }
}

void TitleBar::mouseMoveEvent(QMouseEvent* event) {
    if (!window()->isFullScreen()) {
        if (!m_dragging) return;
        window()->move(event->globalPosition().toPoint() - m_dragStartPosition);
    } else if (!m_doubleclicked) {
        m_dragging = false;
        window()->showNormal();
        m_window->restoreWindowedSize();
        window()->move(event->globalPosition().toPoint().x() - static_cast<int>(m_grabRatio * window()->width()),
                       event->globalPosition().toPoint().y() - height() / 2);
        m_dragStartPosition = event->globalPosition().toPoint() - window()->frameGeometry().topLeft();
        m_dragging = true;
    }
}

void TitleBar::mouseReleaseEvent(QMouseEvent* event) {
    m_dragging = false;
    m_doubleclicked = false;
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        m_doubleclicked = true;
        if (window()->isFullScreen()) {
            window()->showNormal();
        } else {
            window()->showFullScreen();
        }
    }
}
