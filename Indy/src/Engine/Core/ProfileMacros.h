#pragma once

#include <memory>

import Indy_Core;
 
#define INDY_PROFILE_SCOPE(name) std::unique_ptr<Indy::ScopeProfiler> Indy_Profile_Scope = std::make_unique<Indy::ScopeProfiler>(name);