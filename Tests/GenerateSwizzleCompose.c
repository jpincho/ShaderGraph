#include "../ShaderGraph.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main ( void )
	{
	sgGraphHandle Graph = sgCreate();
	if ( Graph == sgGraphHandle_Invalid )
		return 1;

	sgNodeHandle Position3 = sgAddAttribute ( Graph, "Position", sgValueType_Vec3 );
	sgNodeHandle SwizzleX = sgAddSwizzle ( Graph, "x", sgValueType_Vec3, sgValueType_Float );
	sgNodeHandle SwizzleY = sgAddSwizzle ( Graph, "y", sgValueType_Vec3, sgValueType_Float );
	sgNodeHandle SwizzleZ = sgAddSwizzle ( Graph, "z", sgValueType_Vec3, sgValueType_Float );
	sgNodeHandle One = sgAddConstantFloat ( Graph, 1.0f );
	sgNodeHandle Compose = sgAddComposeVec4 ( Graph );
	sgNodeHandle VertexPosition = sgAddVertexPosition ( Graph );
	sgNodeHandle FragmentColor = sgAddFragmentColor ( Graph );
	sgNodeHandle Color = sgAddConstantVec4 ( Graph, 0.2f, 0.4f, 0.6f, 1.0f );
	if ( ( Position3 == sgNodeHandle_Invalid ) || ( SwizzleX == sgNodeHandle_Invalid ) ||
	        ( SwizzleY == sgNodeHandle_Invalid ) || ( SwizzleZ == sgNodeHandle_Invalid ) ||
	        ( One == sgNodeHandle_Invalid ) || ( Compose == sgNodeHandle_Invalid ) ||
	        ( VertexPosition == sgNodeHandle_Invalid ) || ( FragmentColor == sgNodeHandle_Invalid ) ||
	        ( Color == sgNodeHandle_Invalid ) )
		return 1;

	if ( sgAddConnection ( Graph, Position3, 0, SwizzleX, 0 ) == false ) return 1;
	if ( sgAddConnection ( Graph, Position3, 0, SwizzleY, 0 ) == false ) return 1;
	if ( sgAddConnection ( Graph, Position3, 0, SwizzleZ, 0 ) == false ) return 1;
	if ( sgAddConnection ( Graph, SwizzleX, 0, Compose, 0 ) == false ) return 1;
	if ( sgAddConnection ( Graph, SwizzleY, 0, Compose, 1 ) == false ) return 1;
	if ( sgAddConnection ( Graph, SwizzleZ, 0, Compose, 2 ) == false ) return 1;
	if ( sgAddConnection ( Graph, One, 0, Compose, 3 ) == false ) return 1;
	if ( sgAddConnection ( Graph, Compose, 0, VertexPosition, 0 ) == false ) return 1;
	if ( sgAddConnection ( Graph, Color, 0, FragmentColor, 0 ) == false ) return 1;

	char *VS = NULL;
	char *FS = NULL;
	if ( sgGenerateGLSL ( Graph, &VS, &FS ) == false )
		return 1;
	if ( ( strstr ( VS, ".x" ) == NULL ) || ( strstr ( VS, "vec4" ) == NULL ) || ( strstr ( VS, "gl_Position" ) == NULL ) )
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
