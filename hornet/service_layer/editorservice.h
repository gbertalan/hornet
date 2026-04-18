class IModelAccessReadWrite;
struct EditorVisibleLinesDTO;

class EditorService
{
public:
    explicit EditorService(IModelAccessReadWrite &modelAccess);
    void storeEditorState(const EditorVisibleLinesDTO &dto);

private:
    IModelAccessReadWrite &m_modelAccess;
};
