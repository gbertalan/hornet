#pragma once

struct EditorKeyPressDTO
{
    enum class SpecialKey {
        None,
        Left,
        Right,
        Up,
        Down,
        Backspace,
        Delete,
        Enter,
        Home,
        End,
        PageUp,
        PageDown,
        Tab,
        CtrlD, // debug
        CtrlA  // select all
    };
    char32_t key;
    SpecialKey specialKey;
    bool ctrl;
    bool shift;
    bool alt;
    explicit EditorKeyPressDTO(char32_t key, SpecialKey specialKey, bool ctrl, bool shift, bool alt)
        : key(key)
        , specialKey(specialKey)
        , ctrl(ctrl)
        , shift(shift)
        , alt(alt)
    {}
};
