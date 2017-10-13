// stdafx.h: включаемый файл дл€ стандартных системных включаемых файлов
// или включаемых файлов дл€ конкретного проекта, которые часто используютс€, но
// не часто измен€ютс€
//

#pragma once

#pragma warning ( disable : 4005 )
#define _CRT_SECURE_NO_WARNINGS

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // »сключите редко используемые компоненты из заголовков Windows
// ‘айлы заголовков Windows:

#include <d3d9.h>
#include <D3DX10.h>

#include <StrSafe.h>

#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <map>
#include <sstream>
using namespace std;

#include <xmmintrin.h>


#ifdef WIN64

#ifdef _mm_empty
#undef _mm_empty
#define _mm_empty()
#endif
#endif

#ifdef _DEBUG
#define ASSERTS
#endif

#include "STLString/StlUtil.h"
#include "debug.h"
#include "common.h"
#include "log.h"

#include "Geometry/Vector3D.h"
#include "Geometry/Matrix.h"
#include "Geometry/BoundBox.h"
#include "Geometry/Plane.h"


