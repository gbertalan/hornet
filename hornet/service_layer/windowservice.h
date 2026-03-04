
class IModelAccessReadWrite;

class WindowService {
public:
    explicit WindowService(IModelAccessReadWrite& modelAccess);
    void saveWindowState(int x, int y, int width, int height, bool isFullscreen);

private:
    IModelAccessReadWrite& m_modelAccess;
    int counter = 0;
};
