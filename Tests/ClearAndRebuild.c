#include "../ShaderGraph.h"
#include <Platform/Logger.h>
#include <stdlib.h>

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
	if ( ( Position == sgNodeHandle_Invalid ) || ( VertexPosition == sgNodeHandle_Invalid ) ||
	        ( FragmentColor == sgNodeHandle_Invalid ) || ( Color == sgNodeHandle_Invalid ) )
		{
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, Position, 0, VertexPosition, 0 ) == false )
		{
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, Color, 0, FragmentColor, 0 ) == false )
		{
		TestResult = -1;
		goto End;
		}

	sgClear ( Graph );
	if ( sgGetNodeCount ( Graph ) != 0 )
		{
		LOG_ERROR ( "Clear should leave zero nodes" );
		TestResult = -1;
		goto End;
		}

	Position = sgAddAttribute ( Graph, "Position", sgValueType_Vec4 );
	VertexPosition = sgAddVertexPosition ( Graph );
	FragmentColor = sgAddFragmentColor ( Graph );
	Color = sgAddConstantVec4 ( Graph, 0.0f, 0.0f, 1.0f, 1.0f );
	if ( ( Position == sgNodeHandle_Invalid ) || ( VertexPosition == sgNodeHandle_Invalid ) ||
	        ( FragmentColor == sgNodeHandle_Invalid ) || ( Color == sgNodeHandle_Invalid ) )
		{
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, Position, 0, VertexPosition, 0 ) == false )
		{
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, Color, 0, FragmentColor, 0 ) == false )
		{
		TestResult = -1;
		goto End;
		}
	if ( sgValidateGraph ( Graph ) == false )
		{
		TestResult = -1;
		goto End;
		}

End:
	LOG_ERROR ( "Test result - %d", TestResult );
	if ( Graph != sgGraphHandle_Invalid )
		sgDestroy ( Graph );
	return TestResult;
	}
