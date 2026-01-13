#pragma once

#include "vcb/vcb.h"
#include <string>
#include <vector>

namespace clm {
    class Phonetics{
        public:
            std::string languageName;
            std::vector<vcb::st_Object> sound_list; /*list of sounds */
            std::vector<vcb::st_Var> var_list; /*list of base vars*/
            Phonetics(std::string languageName);
            Phonetics(std::string languageName, std::vector<vcb::st_Object> sound_list, std::vector<vcb::st_Var> var_list);
    };
}
