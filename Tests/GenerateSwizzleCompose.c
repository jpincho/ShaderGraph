#include "../ShaderGraph.h"
#include <Platform/Logger.h>
#include <stdlib.h>
#include <string.h>

int main ( void )
	{
	char *VertexShader = NULL;
	char *FragmentShader = NULL;
	int TestResult = 0;

	sgGraphHandle Graph = sgCreate();
	if ( Graph == sgGraphHandle_Invalid )
		{
		LOG_ERROR ( "Failed to create graph" );
		TestResult = -1;
		goto End;
		}

	sgNodeHandle Position3 = sgAddAttribute ( Graph, "Position", sgValueType_Vec3 );
	sgNodeHandle SwizzleX = sgAddSwizzle ( Graph, "x", sgValueType_Vec3, sgValueType_Float );
	sgNodeHandle SwizzleY = sgAddSwizzle ( Graph, "y", sgValueType_Vec3, sgValueType_Float );
	sgNodeHandle SwizzleZ = sgAddSwizzle ( Graph, "z", sgValueType_Vec3, sgValueType_Float );
	sgNodeHandle One = sgAddConstantFloat ( Graph, 1.0f );
	sgNodeHandle Compose = sgAddComposeVector ( Graph, sgValueType_Float, 4 );
	sgNodeHandle VertexPosition = sgAddVertexPosition ( Graph );
	sgNodeHandle FragmentColor = sgAddFragmentColor ( Graph );
	sgNodeHandle Color = sgAddConstantVec4 ( Graph, 0.2f, 0.4f, 0.6f, 1.0f );
	if ( ( Position3 == sgNodeHandle_Invalid ) || ( SwizzleX == sgNodeHandle_Invalid ) ||
	        ( SwizzleY == sgNodeHandle_Invalid ) || ( SwizzleZ == sgNodeHandle_Invalid ) ||
	        ( One == sgNodeHandle_Invalid ) || ( Compose == sgNodeHandle_Invalid ) ||
	        ( VertexPosition == sgNodeHandle_Invalid ) || ( FragmentColor == sgNodeHandle_Invalid ) ||
	        ( Color == sgNodeHandle_Invalid ) )
		{
		LOG_ERROR ( "Failed to create nodes" );
		TestResult = -1;
		goto End;
		}

	if ( sgAddConnection ( Graph, Position3, 0, SwizzleX, 0 ) == false )
		{
		LOG_ERROR ( "Failed to connect Position3 to SwizzleX" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, Position3, 0, SwizzleY, 0 ) == false )
		{
		LOG_ERROR ( "Failed to connect Position3 to SwizzleY" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, Position3, 0, SwizzleZ, 0 ) == false )
		{
		LOG_ERROR ( "Failed to connect Position3 to SwizzleZ" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, SwizzleX, 0, Compose, 0 ) == false )
		{
		LOG_ERROR ( "Failed to connect SwizzleX to Compose" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, SwizzleY, 0, Compose, 1 ) == false )
		{
		LOG_ERROR ( "Failed to connect SwizzleY to Compose" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, SwizzleZ, 0, Compose, 2 ) == false )
		{
		LOG_ERROR ( "Failed to connect SwizzleZ to Compose" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, One, 0, Compose, 3 ) == false )
		{
		LOG_ERROR ( "Failed to connect One to Compose" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, Compose, 0, VertexPosition, 0 ) == false )
		{
		LOG_ERROR ( "Failed to connect Compose to VertexPosition" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, Color, 0, FragmentColor, 0 ) == false )
		{
		LOG_ERROR ( "Failed to connect Color to FragmentColor" );
		TestResult = -1;
		goto End;
		}

	if ( sgGenerateGLSL ( Graph, &VertexShader, &FragmentShader ) == false )
		{
		LOG_ERROR ( "Failed to generate GLSL" );
		TestResult = -1;
		goto End;
		}
	if ( ( strstr ( VertexShader, ".x" ) == NULL ) || ( strstr ( VertexShader, "vec4" ) == NULL ) || ( strstr ( VertexShader, "gl_Position" ) == NULL ) )
		{
		LOG_ERROR ( "Unexpected vertex shader:\n%s\n", VertexShader );
		TestResult = -1;
		goto End;
		}

End:
	LOG_ERROR ( "Test result - %d", TestResult );
	SAFE_DEL_C ( VertexShader );
	SAFE_DEL_C ( FragmentShader );
	if ( Graph != sgGraphHandle_Invalid )
		sgDestroy ( Graph );
	return TestResult;
	}
