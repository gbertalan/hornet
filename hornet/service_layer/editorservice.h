#include <QString>
#include "model_layer/markrange.h"
#include <string>
#include <vector>

class IModelAccessReadWrite;
struct EditorVisibleLinesDTO;
struct EditorCursorPosDTO;
struct EditorKeyPressDTO;
struct EditorSelectionDTO;

class EditorService
{
public:
    explicit EditorService(IModelAccessReadWrite &modelAccess);
    void init();
    void storeEditorState(const EditorVisibleLinesDTO &dto);
    std::vector<std::u32string> retrieveActiveLines();
    void storeTextLines(std::vector<std::u32string> textLines, std::string fileType);
    void storeCursorPos(const EditorCursorPosDTO &dto);
    void storeSelection(const EditorSelectionDTO &dto);
    void insertCharacter(char32_t character);
    void moveCursor(const EditorKeyPressDTO &dto);
    void deleteCharacter(const EditorKeyPressDTO &dto);
    void insertNewLine();
    void insertTab();
    void deleteWordLeft();
    void deleteWordRight();
    bool isTerminal() const;
    void setIsTerminal(bool isTerminal);
    void selectAll();
    QString getSelectedText() const;
    void copySelection();
    void deleteSelectionInternal();
    void pasteFromClipboard();
    void cutSelection();
    void storeMarks(const std::vector<MarkRange> &marks);

private:
    IModelAccessReadWrite &m_modelAccess;

    void moveCursorWordRight(std::vector<std::u32string> &lines, int &cursorX, int &cursorY);
    void moveCursorWordLeft(std::vector<std::u32string> &lines, int &cursorX, int &cursorY);
    void shiftMarksForLinesInserted(int atLine, int count);
    void shiftMarksForLineDeleted(int deletedLine);
};
