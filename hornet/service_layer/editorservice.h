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
    void storeEditorState(const EditorVisibleLinesDTO &dto);
    std::vector<std::u32string> retrieveActiveLines();
    void setTextLines(std::vector<std::u32string> textLines, std::string fileType);
    void storeCursorPos(const EditorCursorPosDTO &dto);
    void insertCharacter(char32_t character);
    void moveCursor(const EditorKeyPressDTO &dto);
    void deleteCharacter(const EditorKeyPressDTO &dto);
    void insertNewLine();
    void insertTab();

private:
    IModelAccessReadWrite &m_modelAccess;
};
