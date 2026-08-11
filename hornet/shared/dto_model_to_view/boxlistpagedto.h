#ifndef BOXLISTPAGEDTO_H
#define BOXLISTPAGEDTO_H
#include "shared/dto_model_to_view/boxlistentrydto.h"
#include <vector>
struct BoxListPageDTO
{
    int startIndex;
    int totalCount;
    std::vector<BoxListEntryDTO> entries;
};
#endif // BOXLISTPAGEDTO_H
