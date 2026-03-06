#pragma once

#include <QColor>

class Theme
{
private:
    static const QColor m_almostBlack;
    static const QColor m_almostWhite;
    static const QColor m_darkGrayTranslucent;
    static const QColor m_brightAmber;
    static const QColor m_brightYellow;
    static const QColor m_darkAmber;
    static const QColor m_darkGray;
    static const QColor m_desaturatedTeal;
    static const QColor m_mediumRed;
    static const QColor m_mutedGold;
    static const QColor m_softAmber;
    static const QColor m_warmGray;

public:
    // Background / foundation color
    static const QColor& almostBlack();

    static const QColor& almostWhite();

    // Titlebar color
    static const QColor& darkGrayTranslucent();

    // Base text color (primary amber)
    static const QColor& brightAmber();

    // Active border
    static const QColor& brightYellow();

    // Keywords (bold weight only) - same as brightAmber but bold
    static const QColor& darkAmber();

    // Main border
    static const QColor& darkGray();

    // Numbers (muted gold) and accent color
    static const QColor& desaturatedTeal();

    // Close button
    static const QColor& mediumRed();

    // Functions (lighter, softer amber)
    static const QColor& mutedGold();

    // Strings (slightly darker amber)
    static const QColor& softAmber();

    // Button hover fill
    static const QColor& warmGray();
};
