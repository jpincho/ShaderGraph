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
	sgNodeHandle Normal = sgAddAttribute ( Graph, "Normal", sgValueType_Vec3 );
	sgNodeHandle Normalize = sgAddNormalize ( Graph );
	sgNodeHandle Compose = sgAddComposeVec4 ( Graph );
	sgNodeHandle Nx = sgAddSwizzle ( Graph, "x", sgValueType_Vec3, sgValueType_Float );
	sgNodeHandle Ny = sgAddSwizzle ( Graph, "y", sgValueType_Vec3, sgValueType_Float );
	sgNodeHandle Nz = sgAddSwizzle ( Graph, "z", sgValueType_Vec3, sgValueType_Float );
	sgNodeHandle One = sgAddConstantFloat ( Graph, 1.0f );
	sgNodeHandle VertexPosition = sgAddVertexPosition ( Graph );
	sgNodeHandle FragmentColor = sgAddFragmentColor ( Graph );
	if ( ( Position == sgNodeHandle_Invalid ) || ( Normal == sgNodeHandle_Invalid ) || ( Normalize == sgNodeHandle_Invalid ) ||
	        ( Compose == sgNodeHandle_Invalid ) || ( Nx == sgNodeHandle_Invalid ) || ( Ny == sgNodeHandle_Invalid ) ||
	        ( Nz == sgNodeHandle_Invalid ) || ( One == sgNodeHandle_Invalid ) ||
	        ( VertexPosition == sgNodeHandle_Invalid ) || ( FragmentColor == sgNodeHandle_Invalid ) )
		return 1;

	if ( sgAddConnection ( Graph, Position, 0, VertexPosition, 0 ) == false ) return 1;
	if ( sgAddConnection ( Graph, Normal, 0, Normalize, 0 ) == false ) return 1;
	if ( sgAddConnection ( Graph, Normalize, 0, Nx, 0 ) == false ) return 1;
	if ( sgAddConnection ( Graph, Normalize, 0, Ny, 0 ) == false ) return 1;
	if ( sgAddConnection ( Graph, Normalize, 0, Nz, 0 ) == false ) return 1;
	if ( sgAddConnection ( Graph, Nx, 0, Compose, 0 ) == false ) return 1;
	if ( sgAddConnection ( Graph, Ny, 0, Compose, 1 ) == false ) return 1;
	if ( sgAddConnection ( Graph, Nz, 0, Compose, 2 ) == false ) return 1;
	if ( sgAddConnection ( Graph, One, 0, Compose, 3 ) == false ) return 1;
	if ( sgAddConnection ( Graph, Compose, 0, FragmentColor, 0 ) == false ) return 1;

	char *VS = NULL;
	char *FS = NULL;
	if ( sgGenerateGLSL ( Graph, &VS, &FS ) == false )
		return 1;
	if ( strstr ( FS, "normalize" ) == NULL )
		{
		fprintf ( stderr, "Fragment shader missing normalize:\n%s\n", FS );
		free ( VS ); free ( FS ); sgDestroy ( Graph );
		return 1;
		}

	free ( VS );
	free ( FS );
	sgDestroy ( Graph );
	return 0;
	}
