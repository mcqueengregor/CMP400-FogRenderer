/*
* Copyright 2014-2021 NVIDIA Corporation.  All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#pragma once

#include "NsightPerfSDK/nvperf_host.h"
#include "NsightPerfSDK/nvperf_target.h"
#include <string>
#include <vector>

namespace nv { namespace perf {
    inline size_t CounterDataGetNumRanges(const uint8_t* pCounterDataImage)
    {
        NVPW_CounterData_GetNumRanges_Params getNumRangeParams = { NVPW_CounterData_GetRangeDescriptions_Params_STRUCT_SIZE };
        getNumRangeParams.pCounterDataImage = pCounterDataImage;
        NVPA_Status nvpaStatus = NVPW_CounterData_GetNumRanges(&getNumRangeParams);
        if (nvpaStatus)
        {
            return 0;
        }
        return getNumRangeParams.numRanges;
    }

    // TODO: this function performs dynamic allocations; either need a non-malloc'ing variant, or move this to an appropriate place
    inline std::string CounterDataGetRangeName(const uint8_t* pCounterDataImage, size_t rangeIndex, char delimiter, const char** ppLeafName = nullptr)
    {
        std::string rangeName;

        NVPW_CounterData_GetRangeDescriptions_Params params = { NVPW_CounterData_GetRangeDescriptions_Params_STRUCT_SIZE };
        params.pCounterDataImage = pCounterDataImage;
        params.rangeIndex = rangeIndex;
        NVPA_Status nvpaStatus = NVPW_CounterData_GetRangeDescriptions(&params);
        if (nvpaStatus)
        {
            return "";
        }

        if (!params.numDescriptions)
        {
            return "";
        }

        std::vector<const char*> descriptions;
        descriptions.resize(params.numDescriptions);
        params.ppDescriptions = descriptions.data();
        nvpaStatus = NVPW_CounterData_GetRangeDescriptions(&params);
        if (nvpaStatus)
        {
            return "";
        }

        rangeName += descriptions[0];
        for (size_t descriptionIdx = 1; descriptionIdx < params.numDescriptions; ++descriptionIdx)
        {
            const char* pDescription = params.ppDescriptions[descriptionIdx];
            rangeName += delimiter;
            rangeName += pDescription;
        }

        if (ppLeafName)
        {
            *ppLeafName = descriptions.back();
        }

        return rangeName;
    }
}}
