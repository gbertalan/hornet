#ifndef RENDERSCRIPTDTO_H
#define RENDERSCRIPTDTO_H
#include "shared/dto_model_to_view/rendercircledto.h"
#include "shared/dto_model_to_view/renderlinedto.h"
#include "shared/dto_model_to_view/renderrectdto.h"
#include "shared/dto_model_to_view/rendertextdto.h"
#include <vector>
struct RenderScriptDTO
{
    std::vector<RenderLineDTO> lines;
    std::vector<RenderRectDTO> rects;
    std::vector<RenderCircleDTO> circles;
    std::vector<RenderTextDTO> texts;
    RenderScriptDTO() = default;
    RenderScriptDTO(const std::vector<RenderLineDTO> &lines,
                    const std::vector<RenderRectDTO> &rects,
                    const std::vector<RenderCircleDTO> &circles,
                    const std::vector<RenderTextDTO> &texts)
        : lines(lines)
        , rects(rects)
        , circles(circles)
        , texts(texts)
    {}
};
#endif // RENDERSCRIPTDTO_H
