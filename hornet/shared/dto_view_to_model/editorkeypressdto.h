#pragma once

struct EditorKeyPressDTO
{
    char32_t key;
    bool ctrl;
    bool shift;
    bool alt;
    explicit EditorKeyPressDTO(char32_t key, bool ctrl, bool shift, bool alt)
        : key(key)
        , ctrl(ctrl)
        , shift(shift)
        , alt(alt)
    {}
};
