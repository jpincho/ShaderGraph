#include "../ShaderGraph.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main ( void )
	{
	sgGraphHandle Graph = sgCreate();
	if ( Graph == sgGraphHandle_Invalid )
		return 1;

	sgNodeHandle Position = sgAddAttribute ( Graph, "Position", sgValueType_Vec4 );
	sgNodeHandle UV = sgAddAttribute ( Graph, "UV", sgValueType_Vec2 );
	sgNodeHandle Sampler = sgAddUniform ( Graph, "AlbedoMap", sgValueType_Sampler2D );
	sgNodeHandle Sample = sgAddTextureSample ( Graph );
	sgNodeHandle VertexPosition = sgAddVertexPosition ( Graph );
	sgNodeHandle FragmentColor = sgAddFragmentColor ( Graph );
	if ( ( Position == sgNodeHandle_Invalid ) || ( UV == sgNodeHandle_Invalid ) || ( Sampler == sgNodeHandle_Invalid ) ||
	        ( Sample == sgNodeHandle_Invalid ) || ( VertexPosition == sgNodeHandle_Invalid ) || ( FragmentColor == sgNodeHandle_Invalid ) )
		return 1;

	if ( sgAddConnection ( Graph, Position, 0, VertexPosition, 0 ) == false ) return 1;
	if ( sgAddConnection ( Graph, Sampler, 0, Sample, 0 ) == false ) return 1;
	if ( sgAddConnection ( Graph, UV, 0, Sample, 1 ) == false ) return 1;
	if ( sgAddConnection ( Graph, Sample, 0, FragmentColor, 0 ) == false ) return 1;

	char *VS = NULL;
	char *FS = NULL;
	if ( sgGenerateGLSL ( Graph, &VS, &FS ) == false )
		return 1;
	if ( ( strstr ( FS, "texture" ) == NULL ) || ( strstr ( FS, "uniform sampler2D AlbedoMap" ) == NULL ) ||
	        ( strstr ( FS, "in vec2 v_UV" ) == NULL ) )
		{
		fprintf ( stderr, "Unexpected fragment shader:\n%s\n", FS );
		free ( VS ); free ( FS ); sgDestroy ( Graph );
		return 1;
		}
	if ( ( strstr ( VS, "out vec2 v_UV" ) == NULL ) || ( strstr ( VS, "v_UV = UV" ) == NULL ) )
		{
		fprintf ( stderr, "Unexpected vertex shader:\n%s\n", VS );
		free ( VS ); free ( FS ); sgDestroy ( Graph );
		return 1;
		}

	free ( VS );
	free ( FS );
	sgDestroy ( Graph );
	return 0;
	}
