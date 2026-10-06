#include "../ShaderGraph.h"

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
	sgGraphHandle Graph = sgCreate ();
	if ( Graph == sgGraphHandle_Invalid )
		return 1;

	sgNodeHandle Position = sgAddAttribute ( Graph, "Position", sgValueType_Vec4 );
	sgNodeHandle VertexPosition = sgAddVertexPosition ( Graph );
	sgNodeHandle Diffuse = sgAddConstantVec4 ( Graph, 1.0f, 0.2f, 0.2f, 1.0f );
	sgNodeHandle Specular = sgAddConstantVec4 ( Graph, 1.0f, 1.0f, 1.0f, 1.0f );
	sgNodeHandle Shininess = sgAddConstantFloat ( Graph, 32.0f );
	sgNodeHandle Normal = sgAddAttribute ( Graph, "Normal", sgValueType_Vec3 );
	sgNodeHandle WorldPosition = sgAddSwizzle ( Graph, "xyz", sgValueType_Vec4, sgValueType_Vec3 );
	sgNodeHandle CameraPosition = sgAddUniform ( Graph, "CameraPosition", sgValueType_Vec3 );
	sgNodeHandle Lit = sgAddBlinnPhong ( Graph );
	sgNodeHandle FragmentColor = sgAddFragmentColor ( Graph );

	if ( ( Position == sgNodeHandle_Invalid ) || ( VertexPosition == sgNodeHandle_Invalid ) ||
	        ( Diffuse == sgNodeHandle_Invalid ) || ( Specular == sgNodeHandle_Invalid ) ||
	        ( Shininess == sgNodeHandle_Invalid ) || ( Normal == sgNodeHandle_Invalid ) ||
	        ( WorldPosition == sgNodeHandle_Invalid ) || ( CameraPosition == sgNodeHandle_Invalid ) ||
	        ( Lit == sgNodeHandle_Invalid ) || ( FragmentColor == sgNodeHandle_Invalid ) )
		return Fail ( Graph, NULL, NULL, "Failed to create nodes" );

	if ( sgAddConnection ( Graph, Position, 0, VertexPosition, 0 ) == false )
		return Fail ( Graph, NULL, NULL, "Position to VertexPosition failed" );
	if ( sgAddConnection ( Graph, Position, 0, WorldPosition, 0 ) == false )
		return Fail ( Graph, NULL, NULL, "Position to WorldPosition failed" );
	if ( ( sgAddConnection ( Graph, Diffuse, 0, Lit, 0 ) == false ) ||
	        ( sgAddConnection ( Graph, Specular, 0, Lit, 1 ) == false ) ||
	        ( sgAddConnection ( Graph, Shininess, 0, Lit, 2 ) == false ) ||
	        ( sgAddConnection ( Graph, Normal, 0, Lit, 3 ) == false ) ||
	        ( sgAddConnection ( Graph, WorldPosition, 0, Lit, 4 ) == false ) ||
	        ( sgAddConnection ( Graph, CameraPosition, 0, Lit, 5 ) == false ) ||
	        ( sgAddConnection ( Graph, Lit, 0, FragmentColor, 0 ) == false ) )
		return Fail ( Graph, NULL, NULL, "BlinnPhong wiring failed" );

	char *VS = NULL;
	char *FS = NULL;
	if ( sgGenerateGLSL ( Graph, &VS, &FS ) == false )
		return Fail ( Graph, NULL, NULL, "GLSL generation failed" );

	if ( strstr ( FS, "EvaluateLights" ) == NULL )
		return Fail ( Graph, VS, FS, "Missing EvaluateLights call" );
	if ( strstr ( FS, "uniform LightBlock" ) == NULL )
		return Fail ( Graph, VS, FS, "Missing LightBlock UBO" );
	if ( ( strstr ( FS, "EvaluatePointLight" ) == NULL ) ||
	        ( strstr ( FS, "EvaluateSpotLight" ) == NULL ) ||
	        ( strstr ( FS, "EvaluateDirectionalLight" ) == NULL ) )
		return Fail ( Graph, VS, FS, "Missing per-light evaluators" );
	if ( strstr ( FS, "AmbientLight" ) == NULL )
		return Fail ( Graph, VS, FS, "Missing ambient light support" );

	free ( VS );
	free ( FS );
	sgDestroy ( Graph );
	return 0;
	}
