#pragma once

struct EditorSettingsDto
{
    int lineHeight;
    float fontScale;

    explicit EditorSettingsDto(int lineHeight, float fontScale)
        : lineHeight(lineHeight)
        , fontScale(fontScale)
    {}
};
