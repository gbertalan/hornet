#pragma once

struct EditorKeyPressDTO
{
    enum class SpecialKey { None, Left, Right, Up, Down, Backspace, Delete };
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
