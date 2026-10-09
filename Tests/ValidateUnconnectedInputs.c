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

	sgNodeHandle VertexPosition = sgAddVertexPosition ( Graph );
	sgNodeHandle FragmentColor = sgAddFragmentColor ( Graph );
	sgNodeHandle Color = sgAddConstantVec4 ( Graph, 1.0f, 0.0f, 0.0f, 1.0f );
	if ( ( VertexPosition == sgNodeHandle_Invalid ) || ( FragmentColor == sgNodeHandle_Invalid ) || ( Color == sgNodeHandle_Invalid ) )
		{
		LOG_ERROR ( "Failed to add nodes" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, Color, 0, FragmentColor, 0 ) == false )
		{
		LOG_ERROR ( "Failed to add connection" );
		TestResult = -1;
		goto End;
		}
	/* VertexPosition input left unconnected. */
	if ( sgValidateGraph ( Graph ) == true )
		{
		LOG_ERROR ( "Unconnected VertexPosition input should not validate" );
		TestResult = -1;
		goto End;
		}

End:
	LOG_ERROR ( "Test result - %d", TestResult );
	if ( Graph != sgGraphHandle_Invalid )
		sgDestroy ( Graph );
	return TestResult;
	}
