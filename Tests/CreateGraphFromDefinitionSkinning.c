#include "../ShaderCreator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int Fail ( sgGraphHandle Graph, char *VS, char *FS, const char *Message )
	{
	fprintf ( stderr, "%s\n", Message );
	free ( VS );
	free ( FS );
	if ( Graph != sgGraphHandle_Invalid )
		sgDestroy ( Graph );
	return 1;
	}

int main ( void )
	{
	scShaderDefinition Definition = {0};
	Definition.DiffuseSource = scMaterialSource_Uniform;
	Definition.NormalSource = scMaterialSource_Off;
	Definition.LightModel = scLightModel_Off;
	Definition.Skinning = true;
	Definition.BoneCount = 0;

	if ( sgCreateGraphFromDefinition ( Definition ) != sgGraphHandle_Invalid )
		{
		fprintf ( stderr, "Skinning with BoneCount 0 should fail\n" );
		return 1;
		}

	Definition.BoneCount = 64;
	sgGraphHandle Graph = sgCreateGraphFromDefinition ( Definition );
	if ( Graph == sgGraphHandle_Invalid )
		return Fail ( Graph, NULL, NULL, "Failed to create skinned graph" );
	if ( sgValidateGraph ( Graph ) == false )
		return Fail ( Graph, NULL, NULL, "Skinned graph failed validation" );

	char *VS = NULL;
	char *FS = NULL;
	if ( sgGenerateGLSL ( Graph, &VS, &FS ) == false )
		return Fail ( Graph, NULL, NULL, "Skinned GLSL generation failed" );
	if ( ( strstr ( VS, "uBoneMatrices" ) == NULL ) || ( strstr ( VS, "BoneIndex" ) == NULL ) )
		return Fail ( Graph, VS, FS, "Vertex shader missing skinning pieces" );

	free ( VS );
	free ( FS );
	sgDestroy ( Graph );
	return 0;
	}
