#include "../ShaderGraph.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main ( void )
	{
	sgGraphHandle Graph = sgCreate();
	if ( Graph == sgGraphHandle_Invalid )
		return 1;

	if ( sgValidateGraph ( Graph ) == true )
		{
		fprintf ( stderr, "Empty graph should not validate\n" );
		sgDestroy ( Graph );
		return 1;
		}

	sgNodeHandle VertexPosition = sgAddVertexPosition ( Graph );
	sgNodeHandle Position = sgAddAttribute ( Graph, "Position", sgValueType_Vec4 );
	if ( ( VertexPosition == sgNodeHandle_Invalid ) || ( Position == sgNodeHandle_Invalid ) )
		return 1;
	if ( sgAddConnection ( Graph, Position, 0, VertexPosition, 0 ) == false )
		return 1;
	if ( sgValidateGraph ( Graph ) == true )
		{
		fprintf ( stderr, "Graph missing FragmentColor should not validate\n" );
		sgDestroy ( Graph );
		return 1;
		}

	sgDestroy ( Graph );
	return 0;
	}
