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
	Definition.DiffuseSource = scMaterialSource_Texture;
	Definition.SpecularSource = scMaterialSource_Uniform;
	Definition.ShininessSource = scMaterialSource_Uniform;
	Definition.NormalSource = scMaterialSource_Attribute;
	Definition.LightModel = scLightModel_BlinnPhong;

	sgGraphHandle Graph = sgCreateGraphFromDefinition ( Definition );
	if ( Graph == sgGraphHandle_Invalid )
		return Fail ( Graph, NULL, NULL, "Failed to create lit graph" );
	if ( sgValidateGraph ( Graph ) == false )
		return Fail ( Graph, NULL, NULL, "Lit graph failed validation" );

	char *VS = NULL;
	char *FS = NULL;
	if ( sgGenerateGLSL ( Graph, &VS, &FS ) == false )
		return Fail ( Graph, NULL, NULL, "Lit GLSL generation failed" );
	if ( strstr ( FS, "EvaluateLights" ) == NULL )
		return Fail ( Graph, VS, FS, "Fragment shader missing EvaluateLights" );
	if ( strstr ( FS, "layout ( std140 ) uniform LightBlock" ) == NULL )
		return Fail ( Graph, VS, FS, "Fragment shader missing LightBlock UBO" );
	if ( ( strstr ( FS, "Points[" ) == NULL ) || ( strstr ( FS, "Spots[" ) == NULL ) ||
	        ( strstr ( FS, "Directionals[" ) == NULL ) || ( strstr ( FS, "Ambient" ) == NULL ) )
		return Fail ( Graph, VS, FS, "Fragment shader missing light arrays" );
	if ( ( strstr ( FS, "DiffuseMap" ) == NULL ) || ( strstr ( FS, "texture" ) == NULL ) )
		return Fail ( Graph, VS, FS, "Fragment shader missing diffuse texture" );
	if ( strstr ( FS, "CameraPosition" ) == NULL )
		return Fail ( Graph, VS, FS, "Fragment shader missing CameraPosition" );

	free ( VS );
	free ( FS );
	sgDestroy ( Graph );
	return 0;
	}
