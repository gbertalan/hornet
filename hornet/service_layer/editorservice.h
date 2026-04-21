#include <string>
#include <vector>

class IModelAccessReadWrite;
struct EditorVisibleLinesDTO;

class EditorService
{
public:
    explicit EditorService(IModelAccessReadWrite &modelAccess);
    void storeEditorState(const EditorVisibleLinesDTO &dto);
    std::vector<std::u32string> retrieveActiveLines();
    void setTextLines(std::vector<std::u32string> textLines);

private:
    IModelAccessReadWrite &m_modelAccess;
};
