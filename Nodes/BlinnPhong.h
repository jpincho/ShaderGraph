#pragma once
#include "../ShaderGraph.h"

unsigned sgGetBlinnPhongLightNodeCount ( const sgGraphHandle GraphHandle );
sgNodeHandle sgAddBlinnPhongNode ( const sgGraphHandle GraphHandle );
bool sgSetBlinnPhongLightCount ( const sgGraphHandle GraphHandle, const sgNodeHandle NodeHandle, const unsigned LightCount );
unsigned sgGetBlinnPhongLightCount ( const sgGraphHandle GraphHandle, const sgNodeHandle NodeHandle );