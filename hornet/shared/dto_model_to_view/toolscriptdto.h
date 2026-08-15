#ifndef RENDERSCRIPTDTO_H
#define RENDERSCRIPTDTO_H
#include "shared/dto_model_to_view/toolcircledto.h"
#include "shared/dto_model_to_view/toollinedto.h"
#include "shared/dto_model_to_view/toolrectdto.h"
#include "shared/dto_model_to_view/tooltextdto.h"
#include <vector>
struct ToolScriptDTO
{
    std::vector<ToolLineDTO> lines;
    std::vector<ToolRectDTO> rects;
    std::vector<ToolCircleDTO> circles;
    std::vector<ToolTextDTO> texts;
    ToolScriptDTO() = default;
    ToolScriptDTO(const std::vector<ToolLineDTO> &lines,
                  const std::vector<ToolRectDTO> &rects,
                  const std::vector<ToolCircleDTO> &circles,
                  const std::vector<ToolTextDTO> &texts)
        : lines(lines)
        , rects(rects)
        , circles(circles)
        , texts(texts)
    {}
};
#endif // RENDERSCRIPTDTO_H
