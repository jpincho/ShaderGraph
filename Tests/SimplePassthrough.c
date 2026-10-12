#include "../ShaderGraph.h"
#include "TestUtils/GLSLangCheck.h"
#include <Platform/Logger.h>
#include <stdlib.h>
#include <string.h>

int main ( void )
	{
	int TestResult = 0;
	char *VertexShader = NULL;
	char *FragmentShader = NULL;

	sgGraphHandle Graph = sgCreate();
	if ( Graph == sgGraphHandle_Invalid )
		{
		LOG_ERROR ( "Failed to create graph" );
		TestResult = -1;
		goto End;
		}


	sgNodeHandle PositionAttributeHandle = sgAddAttribute ( Graph, "Position", sgValueType_Vec4 );
	sgNodeHandle PositionHandle = sgAddVertexPosition ( Graph );
	if ( ( PositionAttributeHandle == sgNodeHandle_Invalid ) || ( PositionHandle == sgNodeHandle_Invalid ) )
		{
		LOG_ERROR ( "Failed to add nodes" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, PositionAttributeHandle, 0, PositionHandle, 0 ) == false )
		{
		LOG_ERROR ( "Failed to add connection" );
		TestResult = -1;
		goto End;
		}

	sgNodeHandle FragmentColorHandle = sgAddFragmentColor ( Graph );
	sgNodeHandle ConstantColorHandle = sgAddConstantVec4 ( Graph, 1.0f, 0.0f, 0.0f, 1.0f );
	if ( ( FragmentColorHandle == sgNodeHandle_Invalid ) || ( ConstantColorHandle == sgNodeHandle_Invalid ) )
		{
		LOG_ERROR ( "Failed to add nodes" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, ConstantColorHandle, 0, FragmentColorHandle, 0 ) == false )
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

	if ( ( strstr ( VertexShader, "gl_Position" ) == NULL ) ||
	        ( strstr ( FragmentShader, "FragColor" ) == NULL ) )
		{
		LOG_ERROR ( "Generated shaders do not contain expected output" );
		TestResult = -1;
		goto End;
		}

End:
	SAFE_DEL_C ( VertexShader );
	SAFE_DEL_C ( FragmentShader );
	if ( Graph != sgGraphHandle_Invalid )
		sgDestroy ( Graph );
	return TestResult;
	}
