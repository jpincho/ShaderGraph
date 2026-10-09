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

	sgNodeHandle Vec3 = sgAddAttribute ( Graph, "Normal", sgValueType_Vec3 );
	sgNodeHandle VertexPosition = sgAddVertexPosition ( Graph );
	if ( ( Vec3 == sgNodeHandle_Invalid ) || ( VertexPosition == sgNodeHandle_Invalid ) )
		{
		LOG_ERROR ( "Failed to add nodes" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, Vec3, 0, VertexPosition, 0 ) == true )
		{
		LOG_ERROR ( "Expected Vec3->Vec4 connection to fail" );
		TestResult = -1;
		goto End;
		}

End:
	LOG_ERROR ( "Test result - %d", TestResult );
	if ( Graph != sgGraphHandle_Invalid )
		sgDestroy ( Graph );
	return TestResult;
	}
