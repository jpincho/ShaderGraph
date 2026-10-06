#include "../ShaderGraph.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main ( void )
	{
	sgGraphHandle Graph = sgCreate();
	if ( Graph == sgGraphHandle_Invalid )
		return 1;

	sgNodeHandle Vec3 = sgAddAttribute ( Graph, "Normal", sgValueType_Vec3 );
	sgNodeHandle VertexPosition = sgAddVertexPosition ( Graph );
	if ( ( Vec3 == sgNodeHandle_Invalid ) || ( VertexPosition == sgNodeHandle_Invalid ) )
		return 1;
	if ( sgAddConnection ( Graph, Vec3, 0, VertexPosition, 0 ) == true )
		{
		fprintf ( stderr, "Expected Vec3->Vec4 connection to fail\n" );
		sgDestroy ( Graph );
		return 1;
		}

	sgDestroy ( Graph );
	return 0;
	}
