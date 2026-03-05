
class IModelAccessReadWrite;
struct WindowDTO;

class WindowService {
public:
    explicit WindowService(IModelAccessReadWrite& modelAccess);
    void saveWindowState(const WindowDTO& dto);

private:
    IModelAccessReadWrite& m_modelAccess;
};
