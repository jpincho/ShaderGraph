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
	Definition.LightModel = scLightModel_Off;
	Definition.NormalSource = scMaterialSource_Off;

	sgGraphHandle Graph = sgCreateGraphFromDefinition ( Definition );
	if ( Graph == sgGraphHandle_Invalid )
		return Fail ( Graph, NULL, NULL, "Failed to create unlit graph" );
	if ( sgValidateGraph ( Graph ) == false )
		return Fail ( Graph, NULL, NULL, "Unlit graph failed validation" );

	char *VS = NULL;
	char *FS = NULL;
	if ( sgGenerateGLSL ( Graph, &VS, &FS ) == false )
		return Fail ( Graph, NULL, NULL, "Unlit GLSL generation failed" );
	if ( ( strstr ( VS, "ModelMatrix" ) == NULL ) || ( strstr ( VS, "gl_Position" ) == NULL ) )
		return Fail ( Graph, VS, FS, "Vertex shader missing transform" );
	if ( ( strstr ( FS, "Diffuse" ) == NULL ) || ( strstr ( FS, "FragColor" ) == NULL ) )
		return Fail ( Graph, VS, FS, "Fragment shader missing diffuse output" );

	free ( VS );
	free ( FS );
	sgDestroy ( Graph );
	return 0;
	}
