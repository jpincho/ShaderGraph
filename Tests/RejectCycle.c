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

	sgNodeHandle A = sgAddMultiplication ( Graph );
	sgNodeHandle B = sgAddAddition ( Graph );
	if ( ( A == sgNodeHandle_Invalid ) || ( B == sgNodeHandle_Invalid ) )
		{
		LOG_ERROR ( "Failed to add nodes" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, A, 0, B, 0 ) == false )
		{
		LOG_ERROR ( "Failed to add connection 1" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, B, 0, A, 0 ) == true )
		{
		LOG_ERROR ( "Failed to add connection 2" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, A, 0, A, 0 ) == true )
		{
		LOG_ERROR ( "Failed to add connection 3" );
		TestResult = -1;
		goto End;
		}

End:
	LOG_ERROR ( "Test result - %d", TestResult );
	if ( Graph != sgGraphHandle_Invalid )
		sgDestroy ( Graph );
	return TestResult;
	}
