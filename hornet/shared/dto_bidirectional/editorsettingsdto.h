#pragma once
struct EditorSettingsDTO
{
    int lineHeight;
    float fontScale;
    bool isTerminal;
    explicit EditorSettingsDTO(int lineHeight, float fontScale, bool isTerminal = false)
        : lineHeight(lineHeight)
        , fontScale(fontScale)
        , isTerminal(isTerminal)
    {}
};
