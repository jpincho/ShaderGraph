#include "../ShaderGraph.h"
#include "TestUtils/GLSLangCheck.h"
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

	sgNodeHandle Position = sgAddAttribute ( Graph, "Position", sgValueType_Vec4 );
	sgNodeHandle VertexPosition = sgAddVertexPosition ( Graph );
	sgNodeHandle Albedo = sgAddConstantVec4 ( Graph, 1.0f, 0.5f, 0.25f, 1.0f );
	sgNodeHandle Tint = sgAddConstantVec4 ( Graph, 0.5f, 0.5f, 0.5f, 1.0f );
	sgNodeHandle Multiply = sgAddMultiplication ( Graph );
	sgNodeHandle FragmentColor = sgAddFragmentColor ( Graph );
	if ( ( Position == sgNodeHandle_Invalid ) || ( VertexPosition == sgNodeHandle_Invalid ) ||
	        ( Albedo == sgNodeHandle_Invalid ) || ( Tint == sgNodeHandle_Invalid ) ||
	        ( Multiply == sgNodeHandle_Invalid ) || ( FragmentColor == sgNodeHandle_Invalid ) )
		{
		LOG_ERROR ( "Failed to create nodes" );
		TestResult = -1;
		goto End;
		}

	if ( sgAddConnection ( Graph, Position, 0, VertexPosition, 0 ) == false )
		{
		LOG_ERROR ( "Failed to connect Position to VertexPosition" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, Albedo, 0, Multiply, 0 ) == false )
		{
		LOG_ERROR ( "Failed to connect Albedo to Multiply" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, Tint, 0, Multiply, 1 ) == false )
		{
		LOG_ERROR ( "Failed to connect Tint to Multiply" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, Multiply, 0, FragmentColor, 0 ) == false )
		{
		LOG_ERROR ( "Failed to connect Multiply to FragmentColor" );
		TestResult = -1;
		goto End;
		}

	if ( sgGenerateGLSL ( Graph, &VertexShader, &FragmentShader ) == false )
		{
		LOG_ERROR ( "Failed to generate GLSL" );
		TestResult = -1;
		goto End;
		}

	if ( sgTestGLSL ( VertexShader, FragmentShader ) == false )
		{
		LOG_ERROR ( "Shader failed to build" );
		TestResult = -1;
		goto End;
		}

	if ( strstr ( FragmentShader, "*" ) == NULL )
		{
		LOG_ERROR ( "Fragment shader missing multiply:\n%s\n", FragmentShader );
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
