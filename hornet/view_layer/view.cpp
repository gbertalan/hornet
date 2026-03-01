#include "view_layer/view.h"
#include "view_layer/window.h"
#include "shared/dto/numberdto.h"

#include <QMessageBox>

View::View(QObject* parent) : QObject(parent) {
    m_window = new Window();
    connect(m_window, &Window::buttonClicked, this, &View::buttonClicked);
}

void View::show() {
    m_window->show();
}

void View::displayNumber(const NumberDTO& dto) {
    m_window->displayNumber(dto);
}

void View::showError(const QString& message) {
    QMessageBox::warning(m_window, "Error", message, QMessageBox::Ok);
}
