#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

struct NumberDTO;

class View : public QWidget {
    Q_OBJECT

public:
    explicit View(QWidget* parent = nullptr);
    void displayNumber(const NumberDTO& dto);
    void showError(const QString& message);

signals:
    void buttonClicked();

private:
    QLineEdit* m_lineEdit;
    QPushButton* m_button;
};
