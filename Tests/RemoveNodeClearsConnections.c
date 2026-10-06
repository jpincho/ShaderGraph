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
	sgNodeHandle Color = sgAddConstantVec4 ( Graph, 1.0f, 0.0f, 0.0f, 1.0f );
	sgNodeHandle Multiply = sgAddMultiply ( Graph );
	if ( ( Position == sgNodeHandle_Invalid ) || ( VertexPosition == sgNodeHandle_Invalid ) ||
	        ( FragmentColor == sgNodeHandle_Invalid ) || ( Color == sgNodeHandle_Invalid ) || ( Multiply == sgNodeHandle_Invalid ) )
		return 1;

	if ( sgAddConnection ( Graph, Position, 0, VertexPosition, 0 ) == false )
		return 1;
	if ( sgAddConnection ( Graph, Color, 0, Multiply, 0 ) == false )
		return 1;
	if ( sgAddConnection ( Graph, Color, 0, Multiply, 1 ) == false )
		return 1;
	if ( sgRemoveNode ( Graph, Multiply ) == false )
		return 1;
	/* Destination pin on a removed node is gone; Color output should still be connectable to FragmentColor. */
	if ( sgAddConnection ( Graph, Color, 0, FragmentColor, 0 ) == false )
		{
		fprintf ( stderr, "Connection after removing consumer failed\n" );
		sgDestroy ( Graph );
		return 1;
		}
	if ( sgValidateGraph ( Graph ) == false )
		return 1;

	sgDestroy ( Graph );
	return 0;
	}
