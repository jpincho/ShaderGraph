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
	sgNodeHandle Color = sgAddConstantVec4 ( Graph, 1.0f, 0.0f, 0.0f, 1.0f );
	if ( ( Position == sgNodeHandle_Invalid ) || ( Color == sgNodeHandle_Invalid ) )
		return 1;

	if ( sgGetNodeCount ( Graph ) != 2 )
		return 1;
	if ( sgGetNodeType ( Graph, Position ) != sgNodeType_Attribute )
		return 1;
	if ( sgGetNodeType ( Graph, Color ) != sgNodeType_Constant )
		return 1;
	if ( strcmp ( sgGetNodeName ( Graph, Position ), "Position" ) != 0 )
		return 1;

	const unsigned PositionId = sgGetNodeId ( Graph, Position );
	if ( sgFindNodeById ( Graph, PositionId ) != Position )
		return 1;
	if ( sgGetNodeId ( Graph, sgNodeHandle_Invalid ) != 0 )
		return 1;
	if ( sgFindNodeByIndex ( Graph, 0 ) == sgNodeHandle_Invalid )
		return 1;
	if ( sgFindNodeByIndex ( Graph, 99 ) != sgNodeHandle_Invalid )
		return 1;
	if ( sgGetNodeType ( Graph, sgNodeHandle_Invalid ) != sgNodeType_Invalid )
		return 1;

	sgDestroy ( Graph );
	return 0;
	}
