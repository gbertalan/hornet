#ifndef TOOLSCRIPTDTO_H
#define TOOLSCRIPTDTO_H
#include "shared/dto_model_to_view/toolbuttondto.h"
#include "shared/dto_model_to_view/toolcircledto.h"
#include "shared/dto_model_to_view/tooldropdowndto.h"
#include "shared/dto_model_to_view/toollinedto.h"
#include "shared/dto_model_to_view/toolrectdto.h"
#include "shared/dto_model_to_view/tooltextdto.h"
#include "shared/dto_model_to_view/tooltextfielddto.h"
#include <vector>
struct ToolScriptDTO
{
    std::vector<ToolLineDTO> lines;
    std::vector<ToolRectDTO> rects;
    std::vector<ToolCircleDTO> circles;
    std::vector<ToolTextDTO> texts;
    std::vector<ToolButtonDTO> buttons;
    std::vector<ToolTextFieldDTO> textFields;
    std::vector<ToolDropdownDTO> dropdowns;
    ToolScriptDTO() = default;
    ToolScriptDTO(const std::vector<ToolLineDTO> &lines,
                  const std::vector<ToolRectDTO> &rects,
                  const std::vector<ToolCircleDTO> &circles,
                  const std::vector<ToolTextDTO> &texts,
                  const std::vector<ToolButtonDTO> &buttons,
                  const std::vector<ToolTextFieldDTO> &textFields,
                  const std::vector<ToolDropdownDTO> &dropdowns)
        : lines(lines)
        , rects(rects)
        , circles(circles)
        , texts(texts)
        , buttons(buttons)
        , textFields(textFields)
        , dropdowns(dropdowns)
    {}
};
#endif // TOOLSCRIPTDTO_H