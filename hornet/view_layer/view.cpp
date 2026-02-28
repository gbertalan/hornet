#include "view.h"
#include "shared/dto/numberdto.h"
#include <QVBoxLayout>
#include <QMessageBox>

View::View(QWidget* parent) : QWidget(parent) {
    m_lineEdit = new QLineEdit(this);
    m_lineEdit->setReadOnly(true);
    m_button = new QPushButton("Double", this);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_lineEdit);
    layout->addWidget(m_button);
    setLayout(layout);
    connect(m_button, &QPushButton::clicked, this, &View::buttonClicked);
}

void View::displayNumber(const NumberDTO& dto) {
    m_lineEdit->setText(QString::number(dto.value));
}

void View::showError(const QString& message) {
    QMessageBox::warning(this, "Error", message, QMessageBox::Ok);
}
