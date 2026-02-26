#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

class View : public QWidget {
    Q_OBJECT
public:
    explicit View(QWidget *parent = nullptr);
    void displayValue(int value);
    void showError(const QString& message);

signals:
    void buttonClicked();

private:
    QLineEdit *m_lineEdit;
    QPushButton *m_button;
};
