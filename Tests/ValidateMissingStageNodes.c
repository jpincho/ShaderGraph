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

	if ( sgValidateGraph ( Graph ) == true )
		{
		fprintf ( stderr, "Empty graph should not validate\n" );
		sgDestroy ( Graph );
		return 1;
		}

	sgNodeHandle VertexPosition = sgAddVertexPosition ( Graph );
	sgNodeHandle Position = sgAddAttribute ( Graph, "Position", sgValueType_Vec4 );
	if ( ( VertexPosition == sgNodeHandle_Invalid ) || ( Position == sgNodeHandle_Invalid ) )
		return 1;
	if ( sgAddConnection ( Graph, Position, 0, VertexPosition, 0 ) == false )
		return 1;
	if ( sgValidateGraph ( Graph ) == true )
		{
		fprintf ( stderr, "Graph missing FragmentColor should not validate\n" );
		sgDestroy ( Graph );
		return 1;
		}

End:
	LOG_ERROR ( "Test result - %d", TestResult );
	if ( Graph != sgGraphHandle_Invalid )
		sgDestroy ( Graph );
	return TestResult;
	}
