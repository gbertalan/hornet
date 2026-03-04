#pragma once

#include <QObject>

struct NumberDTO;
class Window;

class View : public QObject {
    Q_OBJECT

public:
    explicit View(QObject* parent = nullptr);
    void show();
    void displayNumber(const NumberDTO& dto);
    void showError(const QString& message);

signals:
    void buttonClicked();
    void windowStateChanged(int x, int y, int width, int height, bool isFullscreen);
    void debugRequested();

private:
    Window* m_window;
};
