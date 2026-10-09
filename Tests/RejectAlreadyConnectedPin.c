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

	sgNodeHandle A = sgAddConstantVec4 ( Graph, 1.0f, 0.0f, 0.0f, 1.0f );
	sgNodeHandle B = sgAddConstantVec4 ( Graph, 0.0f, 1.0f, 0.0f, 1.0f );
	sgNodeHandle FragmentColor = sgAddFragmentColor ( Graph );
	if ( ( A == sgNodeHandle_Invalid ) || ( B == sgNodeHandle_Invalid ) || ( FragmentColor == sgNodeHandle_Invalid ) )
		{
		LOG_ERROR ( "Failed to add nodes" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, A, 0, FragmentColor, 0 ) == false )
		{
		LOG_ERROR ( "Failed to add connection" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, B, 0, FragmentColor, 0 ) == true )
		{
		LOG_ERROR ( "Second connection to same input should fail" );
		TestResult = -1;
		goto End;
		}

End:
	LOG_ERROR ( "Test result - %d", TestResult );
	if ( Graph != sgGraphHandle_Invalid )
		sgDestroy ( Graph );
	return TestResult;
	}
