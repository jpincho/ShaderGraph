#pragma once
#include <Platform/Platform.h>
#include "ShaderGraph.h"

BEGIN_C_DECLARATIONS
typedef enum
	{
	scMaterialSource_Off = 0,
	scMaterialSource_Uniform,
	scMaterialSource_Attribute,
	scMaterialSource_Texture
	} scMaterialSource;

typedef enum
	{
	scLightModel_Off = 0,
	scLightModel_BlinnPhong
	} scLightModel;

typedef struct
	{
	scMaterialSource DiffuseSource, SpecularSource, ShininessSource, EmissivenessSource, NormalSource;
	scLightModel LightModel;
	unsigned BoneCount;
	bool Skinning;
	} scShaderDefinition;

sgGraphHandle sgCreateGraphFromDefinition ( const scShaderDefinition Definition );
END_C_DECLARATIONS