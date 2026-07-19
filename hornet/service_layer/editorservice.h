#include <string>
#include <vector>

class IModelAccessReadWrite;
struct EditorVisibleLinesDTO;
struct EditorCursorPosDTO;
struct EditorKeyPressDTO;

class EditorService
{
public:
    explicit EditorService(IModelAccessReadWrite &modelAccess);
    void init();
    void storeEditorState(const EditorVisibleLinesDTO &dto);
    std::vector<std::u32string> retrieveActiveLines();
    void storeTextLines(std::vector<std::u32string> textLines, std::string fileType);
    void storeCursorPos(const EditorCursorPosDTO &dto);
    void insertCharacter(char32_t character);
    void moveCursor(const EditorKeyPressDTO &dto);
    void deleteCharacter(const EditorKeyPressDTO &dto);
    void insertNewLine();
    void insertTab();
    void deleteWordLeft();
    void deleteWordRight();
    bool isTerminal() const;
    void setIsTerminal(bool isTerminal);

private:
    IModelAccessReadWrite &m_modelAccess;

    void moveCursorWordRight(std::vector<std::u32string> &lines, int &cursorX, int &cursorY);
    void moveCursorWordLeft(std::vector<std::u32string> &lines, int &cursorX, int &cursorY);
};
