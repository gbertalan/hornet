#include <string>
#include <vector>

class IModelAccessReadWrite;
struct EditorVisibleLinesDTO;
struct EditorUserInputDTO;

class EditorService
{
public:
    explicit EditorService(IModelAccessReadWrite &modelAccess);
    void storeEditorState(const EditorVisibleLinesDTO &dto);
    std::vector<std::u32string> retrieveActiveLines();
    void setTextLines(std::vector<std::u32string> textLines, std::string fileType);
    void storeUserInput(const EditorUserInputDTO &dto);

private:
    IModelAccessReadWrite &m_modelAccess;
};
