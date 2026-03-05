#pragma once

#include <QObject>
#include "shared/dto/windowdto.h"

struct NumberDTO;
class Window;

class View : public QObject {
    Q_OBJECT

public:
    explicit View(const WindowDTO& initialState, QObject* parent = nullptr);
    void show();
    void displayNumber(const NumberDTO& dto);
    void showError(const QString& message);

signals:
    void buttonClicked();
    void windowStateChanged(const WindowDTO& dto);
    void debugRequested();

private:
    Window* m_window;
};
