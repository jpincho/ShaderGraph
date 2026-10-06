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
	sgNodeHandle VertexPosition = sgAddVertexPosition ( Graph );
	sgNodeHandle Albedo = sgAddConstantVec4 ( Graph, 1.0f, 0.5f, 0.25f, 1.0f );
	sgNodeHandle Tint = sgAddConstantVec4 ( Graph, 0.5f, 0.5f, 0.5f, 1.0f );
	sgNodeHandle Multiply = sgAddMultiply ( Graph );
	sgNodeHandle FragmentColor = sgAddFragmentColor ( Graph );
	if ( ( Position == sgNodeHandle_Invalid ) || ( VertexPosition == sgNodeHandle_Invalid ) ||
	        ( Albedo == sgNodeHandle_Invalid ) || ( Tint == sgNodeHandle_Invalid ) ||
	        ( Multiply == sgNodeHandle_Invalid ) || ( FragmentColor == sgNodeHandle_Invalid ) )
		return 1;

	if ( sgAddConnection ( Graph, Position, 0, VertexPosition, 0 ) == false ) return 1;
	if ( sgAddConnection ( Graph, Albedo, 0, Multiply, 0 ) == false ) return 1;
	if ( sgAddConnection ( Graph, Tint, 0, Multiply, 1 ) == false ) return 1;
	if ( sgAddConnection ( Graph, Multiply, 0, FragmentColor, 0 ) == false ) return 1;

	char *VS = NULL;
	char *FS = NULL;
	if ( sgGenerateGLSL ( Graph, &VS, &FS ) == false )
		return 1;
	if ( strstr ( FS, "*" ) == NULL )
		{
		fprintf ( stderr, "Fragment shader missing multiply:\n%s\n", FS );
		free ( VS ); free ( FS ); sgDestroy ( Graph );
		return 1;
		}

	free ( VS );
	free ( FS );
	sgDestroy ( Graph );
	return 0;
	}
