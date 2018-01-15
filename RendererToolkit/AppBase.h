#pragma once

#ifdef UWP
#include "CoreWindowAppBase.h"
using AppBase = CoreWindowAppBase;
#else
#include "HWndAppBase.h"
using AppBase = HWndAppBase;
#endif

