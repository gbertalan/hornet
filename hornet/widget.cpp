#include "widget.h"
// #include "registerdisplay.h"
#include "theme.h"

#include <QPushButton>
#include <QPainter>
#include <QPen>
#include <QBitmap>
#include <QVBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QPainterPath>
#include <QPixmap>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("My First C++ Window");
    setWindowFlags(Qt::FramelessWindowHint);

    // main background color:
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(18, 18, 18));
    setPalette(pal);

    // regDisplay = new RegisterDisplay(this);
    // regDisplay->move(50, 50);

    showFullScreen();
}

Widget::~Widget() {}

void Widget::resizeEvent(QResizeEvent *event)
{
    if (!isFullScreen()) {
        QBitmap maskBitmap(size());
        maskBitmap.fill(Qt::color0);

        QPainter maskPainter(&maskBitmap);
        maskPainter.setBrush(Qt::color1);
        maskPainter.setPen(Qt::color1);
        maskPainter.drawRoundedRect(rect(), 17, 17);

        setMask(maskBitmap);
    } else {
        clearMask();
    }

    QWidget::resizeEvent(event);
}

void Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // mock box:
    int arc2 = 5;

    QPen pen2(Theme::darkAmber(), 2);
    pen2.setCapStyle(Qt::RoundCap);
    pen2.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen2);
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(1000, 10, 400, 400, arc2, arc2);

    // mock box2:
    painter.setPen(Theme::darkAmber());
    painter.drawRoundedRect(1200, 1300, 400, 400, arc2, arc2);

    // vert line:
    int vertLinePos = 800;
    QPen vertPen(QColor(38, 38, 38), 1);
    painter.setPen(vertPen);
    painter.drawLine(vertLinePos, 40, vertLinePos, height());

    // titlebar 2:
    painter.setPen(pen2);
    painter.setBrush(QColor(28, 28, 28, 200));
    painter.setPen(Qt::black);
    painter.drawRect(0, 0, width(), 40);

    // titlebar panels:

    // QPainterPath path;
    // path.moveTo(0, 40);
    // path.lineTo(99, 40);
    // path.quadTo(QPointF(110, 40), QPointF(113, 30));
    // path.lineTo(124, 0);
    // path.quadTo(QPointF(124, 0), QPointF(124, 0));

    // QPen penn(Theme::brightYellow(), 2);
    // painter.setPen(penn);
    // painter.setBrush(Qt::NoBrush);
    // painter.drawPath(path);

    // main border:

    int thickness = 2;
    int margin = thickness/2;
    int arc = 15;
    QPen pen(Theme::brightYellow(), thickness);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    // pen.setCosmetic(true);  // 1-pixel wide regardless of zoom
    painter.drawRoundedRect(margin, margin, width() - (margin * 2), height() - (margin * 2), arc, arc);


    QWidget::paintEvent(event);
}
