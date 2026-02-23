#pragma once

#include <QColor>

class Theme
{
private:
    static const QColor m_almostBlack;
    static const QColor m_brightAmber;
    static const QColor m_brightYellow;
    static const QColor m_darkAmber;
    static const QColor m_desaturatedTeal;
    static const QColor m_mutedGold;
    static const QColor m_softAmber;
    static const QColor m_warmGray;

public:
    // Background / foundation color
    static const QColor& almostBlack();

    // Base text color (primary amber)
    static const QColor& brightAmber();

    // Base text color (primary amber)
    static const QColor& brightYellow();

    // Keywords (bold weight only) - same as brightAmber but bold
    static const QColor& darkAmber();

    // Numbers (muted gold) and accent color
    static const QColor& desaturatedTeal();

    // Functions (lighter, softer amber)
    static const QColor& mutedGold();

    // Strings (slightly darker amber)
    static const QColor& softAmber();

    // Comments (desaturated warm gray)
    static const QColor& warmGray();
};
