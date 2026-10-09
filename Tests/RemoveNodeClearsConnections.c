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
	sgNodeHandle VertexPosition = sgAddVertexPosition ( Graph );
	sgNodeHandle FragmentColor = sgAddFragmentColor ( Graph );
	sgNodeHandle Color = sgAddConstantVec4 ( Graph, 1.0f, 0.0f, 0.0f, 1.0f );
	sgNodeHandle Multiply = sgAddMultiplication ( Graph );
	if ( ( Position == sgNodeHandle_Invalid ) || ( VertexPosition == sgNodeHandle_Invalid ) ||
	        ( FragmentColor == sgNodeHandle_Invalid ) || ( Color == sgNodeHandle_Invalid ) || ( Multiply == sgNodeHandle_Invalid ) )
		{
		LOG_ERROR ( "Failed to add nodes" );
		TestResult = -1;
		goto End;
		}

	if ( sgAddConnection ( Graph, Position, 0, VertexPosition, 0 ) == false )
		{
		LOG_ERROR ( "Failed to add connection" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, Color, 0, Multiply, 0 ) == false )
		{
		LOG_ERROR ( "Failed to add connection" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, Color, 0, Multiply, 1 ) == false )
		{
		LOG_ERROR ( "Failed to add connection" );
		TestResult = -1;
		goto End;
		}
	if ( sgRemoveNode ( Graph, Multiply ) == false )
		{
		LOG_ERROR ( "Failed to remove node" );
		TestResult = -1;
		goto End;
		}
	/* Destination pin on a removed node is gone; Color output should still be connectable to FragmentColor. */
	if ( sgAddConnection ( Graph, Color, 0, FragmentColor, 0 ) == false )
		{
		LOG_ERROR ( "Failed to add connection" );
		TestResult = -1;
		goto End;
		}
	if ( sgValidateGraph ( Graph ) == false )
		{
		LOG_ERROR ( "Failed to validate graph" );
		TestResult = -1;
		goto End;
		}

End:
	LOG_ERROR ( "Test result - %d", TestResult );
	if ( Graph != sgGraphHandle_Invalid )
		sgDestroy ( Graph );
	return TestResult;
	}
