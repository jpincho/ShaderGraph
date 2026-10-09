#include "../ShaderGraph.h"
#include <Platform/Logger.h>
#include <stdlib.h>
#include <string.h>

int main ( void )
	{
	int TestResult = 0;
	sgGraphHandle Graph = sgCreate();
	if ( Graph == sgGraphHandle_Invalid )
		{
		LOG_ERROR ( "Failed to create graph" );
		TestResult = -1;
		goto End;
		}

	sgNodeHandle Position = sgAddAttribute ( Graph, "Position", sgValueType_Vec4 );
	sgNodeHandle Color = sgAddConstantVec4 ( Graph, 1.0f, 0.0f, 0.0f, 1.0f );
	if ( ( Position == sgNodeHandle_Invalid ) || ( Color == sgNodeHandle_Invalid ) )
		{
		LOG_ERROR ( "Failed to add nodes" );
		TestResult = -1;
		goto End;
		}

	if ( sgGetNodeCount ( Graph ) != 2 )
		{
		LOG_ERROR ( "Expected 2 nodes" );
		TestResult = -1;
		goto End;
		}
	if ( sgGetNodeType ( Graph, Position ) != sgNodeType_Attribute )
		{
		LOG_ERROR ( "Expected Position to be an attribute" );
		TestResult = -1;
		goto End;
		}
	if ( sgGetNodeType ( Graph, Color ) != sgNodeType_Constant )
		{
		LOG_ERROR ( "Expected Color to be a constant" );
		TestResult = -1;
		goto End;
		}
	if ( strcmp ( sgGetNodeName ( Graph, Position ), "Position" ) != 0 )
		{
		LOG_ERROR ( "Expected Position name to be 'Position'" );
		TestResult = -1;
		goto End;
		}

	const unsigned PositionId = sgGetNodeID ( Graph, Position );
	if ( sgFindNodeByID ( Graph, PositionId ) != Position )
		{
		LOG_ERROR ( "Failed to find node by ID" );
		TestResult = -1;
		goto End;
		}
	if ( sgGetNodeID ( Graph, sgNodeHandle_Invalid ) != 0 )
		{
		LOG_ERROR ( "Expected invalid node ID to be 0" );
		TestResult = -1;
		goto End;
		}
	if ( sgFindNodeByIndex ( Graph, 0 ) == sgNodeHandle_Invalid )
		{
		LOG_ERROR ( "Failed to find node by index" );
		TestResult = -1;
		goto End;
		}
	if ( sgFindNodeByIndex ( Graph, 99 ) != sgNodeHandle_Invalid )
		{
		LOG_ERROR ( "Expected node at index 99 to be invalid" );
		TestResult = -1;
		goto End;
		}
	if ( sgGetNodeType ( Graph, sgNodeHandle_Invalid ) != sgNodeType_Invalid )
		{
		LOG_ERROR ( "Expected invalid node type to be invalid" );
		TestResult = -1;
		goto End;
		}

End:
	LOG_ERROR ( "Test result - %d", TestResult );
	if ( Graph != sgGraphHandle_Invalid )
		sgDestroy ( Graph );
	return TestResult;
	}
