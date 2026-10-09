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

	sgNodeHandle FirstVertex = sgAddVertexPosition ( Graph );
	sgNodeHandle SecondVertex = sgAddVertexPosition ( Graph );
	sgNodeHandle FirstFragment = sgAddFragmentColor ( Graph );
	sgNodeHandle SecondFragment = sgAddFragmentColor ( Graph );
	if ( FirstVertex == sgNodeHandle_Invalid || FirstFragment == sgNodeHandle_Invalid )
		{
		LOG_ERROR ( "Failed to add nodes" );
		TestResult = -1;
		goto End;
		}
	if ( SecondVertex != sgNodeHandle_Invalid )
		{
		LOG_ERROR ( "Second VertexPosition should be rejected" );
		TestResult = -1;
		goto End;
		}
	if ( SecondFragment != sgNodeHandle_Invalid )
		{
		LOG_ERROR ( "Second FragmentColor should be rejected" );
		TestResult = -1;
		goto End;
		}

End:
	LOG_ERROR ( "Test result - %d", TestResult );
	if ( Graph != sgGraphHandle_Invalid )
		sgDestroy ( Graph );
	return TestResult;
	}
