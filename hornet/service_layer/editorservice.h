class IModelAccessReadWrite;
struct EditorVisibleLinesDto;

class EditorService
{
public:
    explicit EditorService(IModelAccessReadWrite &modelAccess);
    void storeEditorState(const EditorVisibleLinesDto &dto);

private:
    IModelAccessReadWrite &m_modelAccess;
};
