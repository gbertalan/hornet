#pragma once

struct EditorSettingsDTO
{
    int lineHeight;
    float fontScale;

    explicit EditorSettingsDTO(int lineHeight, float fontScale)
        : lineHeight(lineHeight)
        , fontScale(fontScale)
    {}
};
