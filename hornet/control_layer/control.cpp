#include "control.h"
#include "model_layer/editormodel.h"
#include "model_layer/imodelaccess_read.h"
#include "model_layer/numbermodel.h"
#include "model_layer/windowmodel.h"
#include "service_layer/editorservice.h"
#include "service_layer/numberservice.h"
#include "service_layer/terminalservice.h"
#include "service_layer/windowservice.h"
#include "shared/dto_view_to_model/editorcursorposdto.h"
#include "shared/dto_view_to_model/editorkeypressdto.h"
#include "shared/dto_view_to_model/numberdto.h"
#include "shared/dto_view_to_model/windowdto.h"
#include "view_layer/view.h"
#include <shared/dto_model_to_view/editorviewstatedto.h>
#include <stdexcept>

Control::Control(IModelAccessRead &modelAccess,
                 NumberService &service,
                 WindowService &windowService,
                 EditorService &editorService,
                 TerminalService &terminalService,
                 View &view)
    : m_modelAccess(modelAccess)
    , m_service(service)
    , m_windowService(windowService)
    , m_editorService(editorService)
    , m_terminalService(terminalService)
    , m_view(view)
{
    m_editorService
        .setTextLines({U"int main()",
                       U"{",
                       U"    std::cout << \"Hello, World!\" << std::endl;",
                       U"    int x = 0;",
                       U"    int y = 10;",
                       U"    for (int i = 0; i < y; i++) {",
                       U"        x += i;",
                       U"        std::cout << \"Iteration: \" << i << std::endl;",
                       U"    }",
                       U"    if (x > 0) {",
                       U"        std::cout << \"x is positive: \" << x << std::endl;",
                       U"    } else {",
                       U"        std::cout << \"x is zero or negative\" << std::endl;",
                       U"    }",
                       U"    std::vector<int> numbers = {1, 2, 3, 4, 5};",
                       U"    for (int num : numbers) {",
                       U"        std::cout << num << \" \";",
                       U"    }",
                       U"    std::cout << std::endl;",
                       U"    auto lambda = [](int a, int b) { return a + b; };",
                       U"    int result = lambda(5, 3);",
                       U"    std::cout << \"Lambda result: \" << result << std::endl;",
                       U"    std::string greeting = \"Hello\";",
                       U"    greeting += \" World!\";",
                       U"    std::cout << greeting << std::endl;",
                       U"    try {",
                       U"        throw std::runtime_error(\"Something went wrong\");",
                       U"    } catch (const std::exception& e) {",
                       U"        std::cout << \"Caught: \" << e.what() << std::endl;",
                       U"    }",
                       U"    std::unique_ptr<int> ptr = std::make_unique<int>(42);",
                       U"    std::cout << *ptr << std::endl;",
                       U"    std::map<std::string, int> ages;",
                       U"    ages[\"Alice\"] = 30;",
                       U"    ages[\"Bob\"] = 25;",
                       U"    for (const auto& pair : ages) {",
                       U"        std::cout << pair.first << \": \" << pair.second << std::endl;",
                       U"    }",
                       U"    std::cout << \"Press any key to continue...\" << std::endl;",
                       U"    std::cin.get();",
                       U"    return 0;",
                       U"}"},
                      "txt");
}

void Control::init()
{
    NumberDTO dto{m_modelAccess.getNumberModel().getValue()};
    m_view.displayNumber(dto);

    m_isTerminal = true;
    if (m_isTerminal) {
        int lastLine = m_modelAccess.getEditorModel().getNoOfLines() - 1;
        EditorCursorPosDTO dto{0, lastLine};
        m_editorService.storeCursorPos(dto);
        sendCursorPosToEditor();
    }
}

void Control::onButtonClicked()
{
    try {
        NumberDTO dto = m_service.doubleNumber();
        m_view.displayNumber(dto);
    } catch (const std::out_of_range &) {
        m_view.showError("Value out of range");
    }
}

void Control::onWindowStateChanged(const WindowDTO &dto)
{
    m_windowService.storeWindowState(dto);
}

void Control::onEditorStateChanged(const EditorVisibleLinesDTO &dto)
{
    m_editorService.storeEditorState(dto);
    sendStateToEditor();
}

/**
 * @brief Control::sendStateToEditor Retrieves visible lines (and metadata) from Model and sends
 * them to View
 */
void Control::sendStateToEditor()
{
    std::vector<std::u32string> lines = m_editorService.retrieveActiveLines();
    if (lines.empty())
        return;

    int noOfAllLines = m_modelAccess.getEditorModel().getNoOfLines();
    int noOfCharsOfLongestLine = m_modelAccess.getEditorModel().getNoOfCharsOfLongestLine();
    std::string fileType = m_modelAccess.getEditorModel().getFileType();

    QVector<QString> qLines;
    qLines.reserve(static_cast<int>(lines.size()));
    for (const std::u32string &line : lines)
        qLines.push_back(QString::fromUcs4(reinterpret_cast<const char32_t *>(line.c_str()),
                                           static_cast<int>(line.size())));

    EditorViewStateDTO dto{qLines,
                           noOfAllLines,
                           noOfCharsOfLongestLine,
                           QString::fromStdString(fileType)};

    m_view.updateEditorState(dto);
}

void Control::onEditorCursorPosChanged(const EditorCursorPosDTO &dto)
{
    if (m_isTerminal) {
        int lastLine = m_modelAccess.getEditorModel().getNoOfLines() - 1;
        EditorCursorPosDTO terminalDto{dto.cursorX, lastLine};
        m_editorService.storeCursorPos(terminalDto);
    } else {
        m_editorService.storeCursorPos(dto);
    }
    sendCursorPosToEditor();
}

void Control::sendCursorPosToEditor()
{
    int cursorX = m_modelAccess.getEditorModel().getCursorX();
    int cursorY = m_modelAccess.getEditorModel().getCursorY();
    EditorCursorPosDTO dtoToSendToView{cursorX, cursorY};
    m_view.updateEditorCursorPos(dtoToSendToView);
}

void Control::onEditorKeyPressed(const EditorKeyPressDTO &dto)
{
    if (m_isTerminal && handleTerminalKeyPress(dto))
        return;
    handleEditorKeyPress(dto);
    sendStateToEditor();
    sendCursorPosToEditor();
}

bool Control::handleTerminalKeyPress(const EditorKeyPressDTO &dto)
{
    if (dto.specialKey == EditorKeyPressDTO::SpecialKey::Up
        || dto.specialKey == EditorKeyPressDTO::SpecialKey::Down) {
        m_terminalService.navigateHistory(dto.specialKey);
        sendStateToEditor();
        sendCursorPosToEditor();
        return true;
    }
    if (dto.specialKey == EditorKeyPressDTO::SpecialKey::Enter) {
        m_terminalService.executeCommand();
        sendStateToEditor();
        sendCursorPosToEditor();
        return true;
    }
    if (dto.specialKey == EditorKeyPressDTO::SpecialKey::Left
        || dto.specialKey == EditorKeyPressDTO::SpecialKey::Right) {
        int cursorX = m_modelAccess.getEditorModel().getCursorX();
        if (dto.specialKey == EditorKeyPressDTO::SpecialKey::Left && cursorX == 0)
            return true;
        m_editorService.moveCursor(dto);
        int lastLine = m_modelAccess.getEditorModel().getNoOfLines() - 1;
        EditorCursorPosDTO cursorDto{m_modelAccess.getEditorModel().getCursorX(), lastLine};
        m_editorService.storeCursorPos(cursorDto);
        sendStateToEditor();
        sendCursorPosToEditor();
        return true;
    }
    return false;
}

void Control::handleEditorKeyPress(const EditorKeyPressDTO &dto)
{
    if (dto.alt)
        return;
    if (dto.specialKey == EditorKeyPressDTO::SpecialKey::None) {
        if (dto.ctrl)
            return;
        m_editorService.insertCharacter(dto.key);
    } else if (dto.specialKey == EditorKeyPressDTO::SpecialKey::Backspace
               || dto.specialKey == EditorKeyPressDTO::SpecialKey::Delete) {
        if (dto.ctrl) {
            if (dto.specialKey == EditorKeyPressDTO::SpecialKey::Backspace)
                m_editorService.deleteWordLeft();
            else
                m_editorService.deleteWordRight();
        } else {
            m_editorService.deleteCharacter(dto);
        }
    } else if (dto.specialKey == EditorKeyPressDTO::SpecialKey::Enter)
        m_editorService.insertNewLine();
    else if (dto.specialKey == EditorKeyPressDTO::SpecialKey::Tab)
        m_editorService.insertTab();
    else
        m_editorService.moveCursor(dto);
}

void Control::onDebugRequested()
{
#ifdef QT_DEBUG
    printModel();
#endif
}

void Control::printModel() const
{
    const WindowModel &windowModel = m_modelAccess.getWindowModel();
    const EditorModel &editorModel = m_modelAccess.getEditorModel();
    qDebug() << "===" << " MODEL STATE" << debugPrintCounter << "===";
    qDebug() << "WindowModel:";
    qDebug() << "    " << "x:" << windowModel.getX() << "y:" << windowModel.getY()
             << "width:" << windowModel.getWidth() << "height:" << windowModel.getHeight()
             << "fullscreen:" << windowModel.isFullscreen();
    qDebug() << "EditorModel:";
    qDebug() << "    " << "noOfVisibleLines:" << editorModel.getNoOfVisibleLines()
             << "topLineIndex:" << editorModel.getTopLineIndex();
    qDebug() << "    " << "noOfLines:" << editorModel.getNoOfLines()
             << "noOfCharsOfLongestLine:" << editorModel.getNoOfCharsOfLongestLine()
             << "fileType:" << editorModel.getFileType();
    qDebug() << "    " << "cursorX:" << editorModel.getCursorX()
             << "cursorY:" << editorModel.getCursorY();
    qDebug() << "=== MODEL STATE END ===";
    qDebug() << "";
    debugPrintCounter++;
}
