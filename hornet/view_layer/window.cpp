#include "view_layer/window.h"
#include <QBitmap>
#include <QEvent>
#include <QKeyEvent>
#include <QMessageBox>
#include <QMouseEvent>
#include <QMoveEvent>
#include <QPainter>
#include <QPalette>
#include <QResizeEvent>
#include <QTimer>
#include <QVBoxLayout>
#include "shared/dto_model_to_view/tooltrustpromptdto.h"
#include "shared/dto_view_to_model/editorselectiondto.h"
#include "shared/dto_view_to_model/tooldropdownactivateddto.h"
#include "shared/dto_view_to_model/tooltextfieldactivateddto.h"
#include "shared/dto_view_to_model/tooltextfieldcommitdto.h"
#include "shared/dto_view_to_model/windowdto.h"
#include "theme.h"
#include "view_layer/fileloaderpanel.h"
#include "view_layer/mainpopup.h"
#include "view_layer/overlaywidget.h"
#include "view_layer/projectsaverpanel.h"
#include "view_layer/resizehandle.h"
#include "view_layer/scriptrunnerpanel.h"
#include "view_layer/splitpane.h"
#include "view_layer/titlebar.h"
#include "view_layer/titlebarfiledropdown.h"
#include "view_layer/tooldropdownentrypanel.h"
#include "view_layer/tooltextfieldentrypanel.h"
#include "view_layer/trustentrypanel.h"
#include <shared/dto_view_to_model/boxunloadrequesteddto.h>

static constexpr int HANDLE_THICKNESS = 6;
static constexpr int CORNER_SIZE = 12;

Window::Window(const WindowDTO& initialState, QWidget* parent) : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint);
    setMinimumSize(300, 200);
    m_windowedWidth = initialState.width;
    m_windowedHeight = initialState.height;
    resize(initialState.width, initialState.height);
    m_windowedX = initialState.x;
    m_windowedY = initialState.y;
    move(initialState.x, initialState.y);

    QPalette palette;
    palette.setColor(QPalette::Window, Theme::almostBlack());
    setPalette(palette);
    setAutoFillBackground(true);

    m_fontAtlas.addFont(":/fonts/JetBrainsMono-Bold.ttf");
    m_fontAtlas.addFont(":/fonts/NotoSansMono-Bold.ttf");
    m_fontAtlas.addFont(":/fonts/NotoSansCJK-Regular.ttc");
    m_fontRenderer = std::make_unique<FontRenderer>(m_fontAtlas);

    m_splitPane = new SplitPane(800, 0, m_fontAtlas, *m_fontRenderer, this);
    m_splitPane->setGeometry(0, 0, width(), height());

    m_titleBar = new TitleBar(m_fontAtlas, *m_fontRenderer, this);
    m_titleBar->raise();

    m_fileDropdown = new TitlebarFileDropdown(m_fontAtlas, *m_fontRenderer, this);
    connect(m_titleBar, &TitleBar::fileNameButtonClicked, this, [this]() {
        const QPoint anchor = m_titleBar->fileNameDropdownAnchor();
        m_fileDropdown->openAt(anchor.x(), anchor.y(), m_titleBar->currentFileName());
    });
    connect(m_fileDropdown,
            &TitlebarFileDropdown::boxListPageRequested,
            this,
            &Window::boxListPageRequested);
    connect(m_fileDropdown, &TitlebarFileDropdown::boxSelected, this, &Window::boxSelected);

    connect(m_splitPane,
            &SplitPane::leftPaneWidthChanged,
            m_titleBar,
            &TitleBar::updateFileNameButtonPosition);
    QTimer::singleShot(0, this, [this]() {
        m_titleBar->updateFileNameButtonPosition(m_splitPane->leftPaneWidth());
    });

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_titleBar);
    layout->addStretch();
    setLayout(layout);

    m_overlayWidget = new OverlayWidget(this);
    m_overlayWidget->setGeometry(0, 0, width(), height());
    m_overlayWidget->raise();

    m_mainPopup = new MainPopup(m_fontAtlas, *m_fontRenderer, this);
    connect(m_mainPopup, &MainPopup::closeRequested, this, &Window::closeMainPopup);

    m_fileLoaderPanel = new FileLoaderPanel(m_fontAtlas, *m_fontRenderer, m_mainPopup->bodyWidget());
    m_fileLoaderPanel->hide();
    connect(m_fileLoaderPanel,
            &FileLoaderPanel::boxListPageRequested,
            this,
            &Window::popupBoxListPageRequested);

    m_scriptRunnerPanel = new ScriptRunnerPanel(m_fontAtlas,
                                                *m_fontRenderer,
                                                m_mainPopup->bodyWidget());
    m_scriptRunnerPanel->hide();
    connect(m_scriptRunnerPanel,
            &ScriptRunnerPanel::boxListPageRequested,
            this,
            &Window::popupBoxListPageRequested);
    connect(m_scriptRunnerPanel,
            &ScriptRunnerPanel::boxRunRequested,
            this,
            &Window::scriptRunnerBoxRunRequested);
    connect(m_scriptRunnerPanel,
            &ScriptRunnerPanel::runRequested,
            this,
            &Window::scriptRunnerRunRequested);

    m_projectSaverPanel = new ProjectSaverPanel(m_fontAtlas,
                                                *m_fontRenderer,
                                                m_mainPopup->bodyWidget());
    m_projectSaverPanel->hide();
    connect(m_projectSaverPanel,
            &ProjectSaverPanel::boxListPageRequested,
            this,
            &Window::popupBoxListPageRequested);
    connect(m_projectSaverPanel,
            &ProjectSaverPanel::saveRequested,
            this,
            &Window::projectSaverSaveRequested);

    m_textFieldEntryPanel = new TextFieldEntryPanel(m_fontAtlas,
                                                    *m_fontRenderer,
                                                    m_mainPopup->bodyWidget());
    m_textFieldEntryPanel->hide();
    connect(m_textFieldEntryPanel,
            &TextFieldEntryPanel::valueCommitted,
            this,
            [this](const QString &value) {
                emit toolTextFieldCommitted(
                    ToolTextFieldCommitDTO(m_activeTextFieldBoxId, m_activeTextFieldName, value));
                closeMainPopup();
            });

    m_dropdownEntryPanel = new DropdownEntryPanel(m_fontAtlas,
                                                  *m_fontRenderer,
                                                  m_mainPopup->bodyWidget());
    m_dropdownEntryPanel->hide();
    connect(m_dropdownEntryPanel,
            &DropdownEntryPanel::valueCommitted,
            this,
            [this](const QString &value) {
                emit toolTextFieldCommitted(
                    ToolTextFieldCommitDTO(m_activeTextFieldBoxId, m_activeTextFieldName, value));
                closeMainPopup();
            });

    m_trustEntryPanel = new TrustEntryPanel(m_fontAtlas, *m_fontRenderer, m_mainPopup->bodyWidget());
    m_trustEntryPanel->hide();
    connect(m_trustEntryPanel, &TrustEntryPanel::trustAllRequested, this, [this]() {
        emit toolTrustAllRequested(BoxUnloadRequestedDTO(m_pendingTrustBoxId));
        closeMainPopup();
    });

    connect(m_titleBar, &TitleBar::fileLoadButtonClicked, this, &Window::openFileLoadPopup);
    connect(m_titleBar, &TitleBar::projectSaveButtonClicked, this, &Window::openProjectSavePopup);
    connect(m_titleBar, &TitleBar::scriptRunButtonClicked, this, &Window::openScriptRunPopup);
    positionMainPopup();

    connect(m_overlayWidget, &OverlayWidget::clickedWhileDimmed, this, &Window::closeMainPopup);

    setupResizeHandles();
    positionResizeHandles();

    if (initialState.isFullscreen)
        showFullScreen();

    connect(m_titleBar, &TitleBar::windowCloseClicked,    this, &Window::windowCloseClicked);
    connect(m_titleBar, &TitleBar::minimizeClicked, this, [this]() {
        showMinimized();
    });
    connect(m_titleBar, &TitleBar::maximizeClicked, this, [this]() {
        if (isFullScreen()) {
            showNormal();
            restoreWindowedSize();
            restoreWindowedLocation();
        } else {
            showFullScreen();
        }
    });

    connect(m_splitPane, &SplitPane::editorStateChanged, this, &Window::editorStateChanged);
    connect(m_splitPane, &SplitPane::editorCursorPosChanged, this, &Window::editorCursorPosChanged);
    connect(m_splitPane, &SplitPane::editorKeyPressed, this, &Window::editorKeyPressed);
    connect(m_splitPane, &SplitPane::editorSelectionChanged, this, &Window::editorSelectionChanged);
    connect(m_splitPane, &SplitPane::gridZoomChanged, this, &Window::gridZoomChanged);
    connect(m_splitPane, &SplitPane::gridDragged, this, &Window::gridDragged);
    connect(m_splitPane, &SplitPane::boxDragged, this, &Window::boxDragged);
    connect(m_splitPane, &SplitPane::boxSelected, this, &Window::boxSelected);
    connect(m_splitPane, &SplitPane::boxResized, this, &Window::boxResized);
    connect(m_splitPane, &SplitPane::boxUnloadRequested, this, &Window::boxUnloadRequested);
    connect(m_splitPane, &SplitPane::toolButtonActivated, this, &Window::toolButtonActivated);
    connect(m_splitPane, &SplitPane::toolTextFieldActivated, this, &Window::openToolTextFieldPopup);
    connect(m_splitPane, &SplitPane::toolDropdownActivated, this, &Window::openToolDropdownPopup);

    connect(m_fileLoaderPanel,
            &FileLoaderPanel::loadRequested,
            this,
            &Window::fileLoaderLoadRequested);
}

void Window::setupResizeHandles() {
    m_handleLeft        = new ResizeHandle(ResizeEdge::Left, this);
    m_handleRight       = new ResizeHandle(ResizeEdge::Right, this);
    m_handleTop         = new ResizeHandle(ResizeEdge::Top, this);
    m_handleBottom      = new ResizeHandle(ResizeEdge::Bottom, this);
    m_handleTopLeft     = new ResizeHandle(ResizeEdge::TopLeft, this);
    m_handleTopRight    = new ResizeHandle(ResizeEdge::TopRight, this);
    m_handleBottomLeft  = new ResizeHandle(ResizeEdge::BottomLeft, this);
    m_handleBottomRight = new ResizeHandle(ResizeEdge::BottomRight, this);
}

void Window::positionResizeHandles() {
    int w = width();
    int h = height();
    int t = HANDLE_THICKNESS;
    int c = CORNER_SIZE;

    m_handleTop->setGeometry(c, 0, w - 2 * c, t);
    m_handleBottom->setGeometry(c, h - t, w - 2 * c, t);
    m_handleLeft->setGeometry(0, c, t, h - 2 * c);
    m_handleRight->setGeometry(w - t, c, t, h - 2 * c);

    m_handleTopLeft->setGeometry(0, 0, c, c);
    m_handleTopRight->setGeometry(w - c, 0, c, c);
    m_handleBottomLeft->setGeometry(0, h - c, c, c);
    m_handleBottomRight->setGeometry(w - c, h - c, c, c);

    m_handleLeft->raise();
    m_handleRight->raise();
    m_handleTop->raise();
    m_handleBottom->raise();
    m_handleTopLeft->raise();
    m_handleTopRight->raise();
    m_handleBottomLeft->raise();
    m_handleBottomRight->raise();
}

void Window::positionMainPopup()
{
    int x = (width() - m_mainPopup->width()) / 2;
    int y = (height() - m_mainPopup->height()) / 2;
    m_mainPopup->move(x, y);
}

void Window::openFileLoadPopup()
{
    m_activePopupListTarget = PopupListTarget::FileLoader;
    m_mainPopup->setHeaderText("FILE LOADER");
    m_scriptRunnerPanel->hide();
    m_projectSaverPanel->hide();
    const int contentHeight = m_fileLoaderPanel->preferredHeight();
    m_mainPopup->setBodyContentHeight(contentHeight);
    m_fileLoaderPanel->setGeometry(0, 0, m_mainPopup->bodyWidget()->width(), contentHeight);
    m_fileLoaderPanel->show();
    m_fileLoaderPanel->refreshLoadedBoxes();
    openMainPopupShared();
}

void Window::openProjectSavePopup()
{
    m_activePopupListTarget = PopupListTarget::ProjectSaver;
    m_mainPopup->setHeaderText("PROJECT SAVER");
    m_fileLoaderPanel->hide();
    m_scriptRunnerPanel->hide();
    const int contentHeight = m_projectSaverPanel->preferredHeight();
    m_mainPopup->setBodyContentHeight(contentHeight);
    m_projectSaverPanel->setGeometry(0, 0, m_mainPopup->bodyWidget()->width(), contentHeight);
    m_projectSaverPanel->show();
    m_projectSaverPanel->refreshBoxes();
    openMainPopupShared();
}

void Window::openScriptRunPopup()
{
    m_activePopupListTarget = PopupListTarget::ScriptRunner;
    m_mainPopup->setHeaderText("SCRIPT RUNNER");
    m_fileLoaderPanel->hide();
    m_projectSaverPanel->hide();
    const int contentHeight = m_scriptRunnerPanel->preferredHeight();
    m_mainPopup->setBodyContentHeight(contentHeight);
    m_scriptRunnerPanel->setGeometry(0, 0, m_mainPopup->bodyWidget()->width(), contentHeight);
    m_scriptRunnerPanel->show();
    m_scriptRunnerPanel->refreshRunnableBoxes();
    openMainPopupShared();
}

void Window::openToolTextFieldPopup(const ToolTextFieldActivatedDTO &dto)
{
    m_activePopupListTarget = PopupListTarget::TextFieldEntry;
    m_activeTextFieldBoxId = dto.boxId;
    m_activeTextFieldName = dto.fieldName;
    m_mainPopup->setHeaderText("SET FIELD");
    m_fileLoaderPanel->hide();
    m_scriptRunnerPanel->hide();
    m_projectSaverPanel->hide();
    m_textFieldEntryPanel->setFieldName(dto.fieldName);
    m_textFieldEntryPanel->setValue(dto.currentValue);
    const int contentHeight = m_textFieldEntryPanel->preferredHeight();
    m_mainPopup->setBodyContentHeight(contentHeight);
    m_textFieldEntryPanel->setGeometry(0, 0, m_mainPopup->bodyWidget()->width(), contentHeight);
    m_textFieldEntryPanel->show();
    openMainPopupShared();
}

void Window::openToolDropdownPopup(const ToolDropdownActivatedDTO &dto)
{
    m_activePopupListTarget = PopupListTarget::DropdownEntry;
    m_activeTextFieldBoxId = dto.boxId;
    m_activeTextFieldName = dto.fieldName;
    m_mainPopup->setHeaderText("SET FIELD");
    m_fileLoaderPanel->hide();
    m_scriptRunnerPanel->hide();
    m_projectSaverPanel->hide();
    m_textFieldEntryPanel->hide();
    m_dropdownEntryPanel->setFieldName(dto.fieldName);
    m_dropdownEntryPanel->setOptions(dto.options, dto.currentValue);
    const int contentHeight = m_dropdownEntryPanel->preferredHeight();
    m_mainPopup->setBodyContentHeight(contentHeight);
    m_dropdownEntryPanel->setGeometry(0, 0, m_mainPopup->bodyWidget()->width(), contentHeight);
    m_dropdownEntryPanel->show();
    openMainPopupShared();
}

void Window::updateToolTrustPrompt(const ToolTrustPromptDTO &dto)
{
    m_activePopupListTarget = PopupListTarget::TrustEntry;
    m_pendingTrustBoxId = dto.boxId;
    m_mainPopup->setHeaderText("TRUST REQUIRED");
    m_fileLoaderPanel->hide();
    m_scriptRunnerPanel->hide();
    m_projectSaverPanel->hide();
    m_textFieldEntryPanel->hide();
    m_dropdownEntryPanel->hide();
    m_trustEntryPanel->setCommands(dto.untrustedCommands, dto.untrustedSourceCommands);
    const int contentHeight = m_trustEntryPanel->preferredHeight();
    m_mainPopup->setBodyContentHeight(contentHeight);
    m_trustEntryPanel->setGeometry(0, 0, m_mainPopup->bodyWidget()->width(), contentHeight);
    m_trustEntryPanel->show();
    openMainPopupShared();
}

void Window::openMainPopupShared()
{
    m_titleBar->setEnabled(false);
    m_splitPane->setEnabled(false);
    m_splitPane->resetGridCtrlState();
    m_overlayWidget->setDimmed(true);
    positionMainPopup();
    m_mainPopup->raise();
    m_mainPopup->show();
    m_mainPopup->setFocus();
}

void Window::closeMainPopup()
{
    m_mainPopup->hide();
    m_overlayWidget->setDimmed(false);
    m_titleBar->setEnabled(true);
    m_splitPane->setEnabled(true);
    m_splitPane->focusEditor();
}

void Window::updateEditorState(const EditorViewStateDTO &dto)
{
    m_splitPane->updateEditorState(dto);
}

void Window::updateEditorCursorPos(const EditorCursorPosDTO &dto)
{
    m_splitPane->updateEditorCursorPos(dto);
}

void Window::updateEditorSettings(const EditorSettingsDTO &dto)
{
    m_splitPane->updateEditorSettings(dto);
}

void Window::updateEditorSelection(const EditorSelectionDTO &dto)
{
    m_splitPane->updateEditorSelection(dto);
}

void Window::updateEditorMarks(const EditorMarksDTO &dto)
{
    m_splitPane->updateEditorMarks(dto);
}

void Window::updateGridViewState(const GridViewStateDTO &dto)
{
    m_splitPane->updateGridViewState(dto);
}

void Window::updateFileName(const QString &fileName)
{
    m_titleBar->updateFileName(fileName);
}

void Window::updateBoxListPage(const BoxListPageDTO &dto)
{
    m_fileDropdown->updateBoxListPage(dto);
}

void Window::updatePopupBoxListPage(const BoxListPageDTO &dto)
{
    if (m_activePopupListTarget == PopupListTarget::ScriptRunner)
        m_scriptRunnerPanel->updateBoxListPage(dto);
    else if (m_activePopupListTarget == PopupListTarget::ProjectSaver)
        m_projectSaverPanel->updateBoxListPage(dto);
    else
        m_fileLoaderPanel->updateBoxListPage(dto);
}

void Window::updateProjectSaverSaveResult(const QString &message)
{
    m_projectSaverPanel->updateSaveResult(message);
}

void Window::updateCurrentBoxId(int boxId)
{
    m_titleBar->updateCurrentBoxId(boxId);
    m_fileDropdown->setCurrentBoxId(boxId);
}

void Window::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    m_splitPane->setGeometry(0, 0, width(), height());
    m_overlayWidget->setGeometry(0, 0, width(), height());

    positionResizeHandles();
    if (m_mainPopup->isVisible())
        m_mainPopup->raise();

    positionMainPopup();

    if (!isFullScreen()) {
        m_windowedWidth = width();
        m_windowedHeight = height();

        int x = this->x();
        int y = this->y();
        int width = this->width();
        int height = this->height();
        bool isFullscreen = false;
        WindowDTO dto{x, y, width, height, isFullscreen};
        emit windowStateChanged(dto);

        QBitmap maskBitmap(size());
        maskBitmap.fill(Qt::color0);
        QPainter maskPainter(&maskBitmap);
        maskPainter.setBrush(Qt::color1);
        maskPainter.setPen(Qt::color1);
        maskPainter.drawRoundedRect(rect(), 17, 17);

        setMask(maskBitmap);
    } else {
        clearMask();
    }
}

void Window::moveEvent(QMoveEvent* event) {
    QWidget::moveEvent(event);
    if (!isFullScreen()){
        m_windowedX = x();
        m_windowedY = y();

        int x = this->x();
        int y = this->y();
        int width = this->width();
        int height = this->height();
        bool isFullscreen = false;
        WindowDTO dto{x, y, width, height, isFullscreen};
        emit windowStateChanged(dto);
    }
}

void Window::changeEvent(QEvent* event) {
    QWidget::changeEvent(event);
    if (event->type() == QEvent::WindowStateChange) {
        bool isFullscreen = isFullScreen();
        m_overlayWidget->setFullscreen(isFullscreen);
        m_titleBar->setFullscreen(isFullscreen);

        m_handleLeft->setVisible(!isFullscreen);
        m_handleRight->setVisible(!isFullscreen);
        m_handleTop->setVisible(!isFullscreen);
        m_handleBottom->setVisible(!isFullscreen);
        m_handleTopLeft->setVisible(!isFullscreen);
        m_handleTopRight->setVisible(!isFullscreen);
        m_handleBottomLeft->setVisible(!isFullscreen);
        m_handleBottomRight->setVisible(!isFullscreen);

        int x = this->x();
        int y = this->y();
        int width = m_windowedWidth;
        int height = m_windowedHeight;
        WindowDTO dto{x, y, width, height, isFullscreen};

        emit windowStateChanged(dto);
    }
    if (event->type() == QEvent::ActivationChange) {
        m_overlayWidget->setFocused(isActiveWindow());
    }
}

// When exiting fullscreen, the size gets reset to the previous size
// when it was still windowed, not fullscreen.
void Window::restoreWindowedSize() {
    resize(m_windowedWidth, m_windowedHeight);
}

void Window::restoreWindowedLocation() {
    move(m_windowedX, m_windowedY);
}




