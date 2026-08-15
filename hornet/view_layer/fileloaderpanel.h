#pragma once
#include <QWidget>
#include "shared/dto_model_to_view/boxlistentrydto.h"
class FontAtlas;
class FontRenderer;
class BoxListPanel;
class QLineEdit;
class QCheckBox;
class QPushButton;
class QListWidget;
class QLabel;
struct BoxListPageDTO;
struct BoxListPageRequestDTO;
struct FilePathListDTO;
class BorderedPanel : public QWidget
{
    Q_OBJECT
public:
    explicit BorderedPanel(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};
class FileLoaderPanel : public QWidget
{
    Q_OBJECT
public:
    explicit FileLoaderPanel(FontAtlas &fontAtlas,
                             FontRenderer &fontRenderer,
                             QWidget *parent = nullptr);
    void updateBoxListPage(const BoxListPageDTO &dto);
    void refreshLoadedBoxes();
    int preferredHeight() const;
signals:
    void boxListPageRequested(const BoxListPageRequestDTO &dto);
    void loadRequested(const FilePathListDTO &dto);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void layoutChildren();
    QString m_monoFamily;
    FontAtlas &m_fontAtlas;
    FontRenderer &m_fontRenderer;

    BorderedPanel *m_loadSectionContainer;
    QPushButton *m_modeFilesButton;
    QPushButton *m_modeDirectoryButton;
    QPushButton *m_browseButton;
    QListWidget *m_pendingFilesList;
    QWidget *m_directoryOptionsRow;
    QLineEdit *m_extensionField;
    QCheckBox *m_recursiveCheckBox;
    QPushButton *m_loadButton;
    QPushButton *m_clearButton;

    BorderedPanel *m_loadedSectionContainer;
    BoxListPanel *m_loadedBoxesList;

    QString m_statusReadoutText;

    int m_loadEyebrowY = 0;
    int m_loadDividerY = 0;
    int m_loadCaptionY = 0;
    int m_loadedEyebrowY = 0;
    int m_loadedDividerY = 0;
    int m_loadedCaptionY = 0;
    int m_statusReadoutY = 0;

    static constexpr int m_listVisibleRows = 5;
    static constexpr int m_margin = 18;
    static constexpr int m_eyebrowHeight = 20;
    static constexpr int m_gapEyebrowToDivider = 6;
    static constexpr int m_gapDividerToCaption = 10;
    static constexpr int m_captionHeight = 14;
    static constexpr int m_gapCaptionToContainer = 10;
    static constexpr int m_gapSection = 26;
    static constexpr int m_gapListToReadout = 14;
    static constexpr int m_readoutHeight = 16;
    static constexpr int m_loadContainerHeight = 300;
};