#pragma once
#include <QWidget>
#include <view_layer/font_renderer/FontAtlas.h>
#include <view_layer/font_renderer/FontRenderer.h>

class EditorSelectionDTO;
struct BoxResizeDTO;
struct BoxSelectedDTO;
struct BoxDragDTO;
struct GridDragDTO;
struct GridViewStateDTO;
struct EditorKeyPressDTO;
struct EditorCursorPosDTO;
struct WindowDTO;
struct EditorVisibleLinesDTO;
struct EditorViewStateDTO;
struct EditorSettingsDTO;
struct GridZoomDTO;
class TitleBar;
class ResizeHandle;
class SplitPane;
class OverlayWidget;
class TitlebarFileDropdown;
class MainPopup;
class FileLoaderPanel;
class ScriptRunnerPanel;
class ProjectSaverPanel;
class TextFieldEntryPanel;
class TrustEntryPanel;
struct ToolTrustPromptDTO;
struct BoxListPageDTO;
struct BoxListPageRequestDTO;
struct FilePathListDTO;
struct BoxUnloadRequestedDTO;
struct ToolButtonActivatedDTO;
struct ToolTextFieldActivatedDTO;
struct ToolTextFieldCommitDTO;
class DropdownEntryPanel;
struct ToolDropdownActivatedDTO;

class Window : public QWidget
{
    Q_OBJECT
public:
    explicit Window(const WindowDTO &initialState, QWidget *parent = nullptr);
    void restoreWindowedSize();
    void restoreWindowedLocation();
    void updateEditorState(const EditorViewStateDTO &dto);
    void updateEditorCursorPos(const EditorCursorPosDTO &dto);
    void updateEditorSettings(const EditorSettingsDTO &dto);
    void updateEditorSelection(const EditorSelectionDTO &dto);
    void updateGridViewState(const GridViewStateDTO &dto);
    void updateFileName(const QString &fileName);
    void updateBoxListPage(const BoxListPageDTO &dto);
    void updatePopupBoxListPage(const BoxListPageDTO &dto);
    void updateProjectSaverSaveResult(const QString &message);
    void updateCurrentBoxId(int boxId);
    void updateToolTrustPrompt(const ToolTrustPromptDTO &dto);

signals:
    void buttonClicked();
    void windowStateChanged(const WindowDTO &dto);
    void windowCloseClicked();
    void editorStateChanged(const EditorVisibleLinesDTO &dto);
    void editorCursorPosChanged(const EditorCursorPosDTO &dto);
    void editorKeyPressed(const EditorKeyPressDTO &dto);
    void editorSelectionChanged(const EditorSelectionDTO &dto);
    void gridZoomChanged(const GridZoomDTO &dto);
    void gridDragged(const GridDragDTO &dto);
    void boxDragged(const BoxDragDTO &dto);
    void boxSelected(const BoxSelectedDTO &dto);
    void boxResized(const BoxResizeDTO &dto);
    void boxUnloadRequested(const BoxUnloadRequestedDTO &dto);
    void toolTrustAllRequested(const BoxUnloadRequestedDTO &dto);
    void toolButtonActivated(const ToolButtonActivatedDTO &dto);
    void toolTextFieldCommitted(const ToolTextFieldCommitDTO &dto);
    void boxListPageRequested(const BoxListPageRequestDTO &dto);
    void popupBoxListPageRequested(const BoxListPageRequestDTO &dto);
    void fileLoaderLoadRequested(const FilePathListDTO &dto);
    void scriptRunnerBoxRunRequested(int boxId);
    void scriptRunnerRunRequested(const FilePathListDTO &dto);
    void projectSaverSaveRequested(const QString &baseName);

private:
    TitleBar *m_titleBar;
    SplitPane *m_splitPane;
    OverlayWidget *m_overlayWidget;
    ResizeHandle *m_handleLeft;
    ResizeHandle *m_handleRight;
    ResizeHandle *m_handleTop;
    ResizeHandle *m_handleBottom;
    ResizeHandle *m_handleTopLeft;
    ResizeHandle *m_handleTopRight;
    ResizeHandle *m_handleBottomLeft;
    ResizeHandle *m_handleBottomRight;
    FontAtlas m_fontAtlas;
    std::unique_ptr<FontRenderer> m_fontRenderer;
    int m_windowedWidth;
    int m_windowedHeight;
    int m_windowedX;
    int m_windowedY;
    // Creates the resize handles, small, transparent panels to handle the edge/corner dragging
    void setupResizeHandles();
    // Places the resize handlers to the edges and corners
    void positionResizeHandles();
    TitlebarFileDropdown *m_fileDropdown;
    MainPopup *m_mainPopup;
    FileLoaderPanel *m_fileLoaderPanel;
    ScriptRunnerPanel *m_scriptRunnerPanel;
    ProjectSaverPanel *m_projectSaverPanel;
    TextFieldEntryPanel *m_textFieldEntryPanel;
    DropdownEntryPanel *m_dropdownEntryPanel;
    TrustEntryPanel *m_trustEntryPanel;
    int m_pendingTrustBoxId = -1;
    int m_activeTextFieldBoxId = -1;
    QString m_activeTextFieldName;
    enum class PopupListTarget {
        FileLoader,
        ScriptRunner,
        ProjectSaver,
        TextFieldEntry,
        DropdownEntry,
        TrustEntry
    };
    PopupListTarget m_activePopupListTarget = PopupListTarget::FileLoader;
    void positionMainPopup();
    void openFileLoadPopup();
    void openProjectSavePopup();
    void openScriptRunPopup();
    void openToolTextFieldPopup(const ToolTextFieldActivatedDTO &dto);
    void openToolDropdownPopup(const ToolDropdownActivatedDTO &dto);
    void openMainPopupShared();
    void closeMainPopup();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void moveEvent(QMoveEvent *event) override;
    // Disable resize when in fullscreen mode
    void changeEvent(QEvent *event) override;
};