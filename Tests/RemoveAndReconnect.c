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
	sgNodeHandle FragmentColor = sgAddFragmentColor ( Graph );
	sgNodeHandle Red = sgAddConstantVec4 ( Graph, 1.0f, 0.0f, 0.0f, 1.0f );
	sgNodeHandle Green = sgAddConstantVec4 ( Graph, 0.0f, 1.0f, 0.0f, 1.0f );
	if ( ( Position == sgNodeHandle_Invalid ) || ( VertexPosition == sgNodeHandle_Invalid ) ||
	        ( FragmentColor == sgNodeHandle_Invalid ) || ( Red == sgNodeHandle_Invalid ) || ( Green == sgNodeHandle_Invalid ) )
		return 1;
	if ( sgAddConnection ( Graph, Position, 0, VertexPosition, 0 ) == false )
		return 1;
	if ( sgAddConnection ( Graph, Red, 0, FragmentColor, 0 ) == false )
		return 1;
	if ( sgRemoveConnection ( Graph, Red, 0, FragmentColor, 0 ) == false )
		return 1;
	if ( sgAddConnection ( Graph, Green, 0, FragmentColor, 0 ) == false )
		{
		fprintf ( stderr, "Reconnect after remove failed\n" );
		sgDestroy ( Graph );
		return 1;
		}
	if ( sgValidateGraph ( Graph ) == false )
		return 1;

	sgDestroy ( Graph );
	return 0;
	}
