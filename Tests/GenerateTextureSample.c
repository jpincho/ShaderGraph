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
	sgNodeHandle UV = sgAddAttribute ( Graph, "UV", sgValueType_Vec2 );
	sgNodeHandle Sampler = sgAddUniform ( Graph, "AlbedoMap", sgValueType_Sampler2D );
	sgNodeHandle Sample = sgAddTextureSample ( Graph );
	sgNodeHandle VertexPosition = sgAddVertexPosition ( Graph );
	sgNodeHandle FragmentColor = sgAddFragmentColor ( Graph );
	if ( ( Position == sgNodeHandle_Invalid ) || ( UV == sgNodeHandle_Invalid ) || ( Sampler == sgNodeHandle_Invalid ) ||
	        ( Sample == sgNodeHandle_Invalid ) || ( VertexPosition == sgNodeHandle_Invalid ) || ( FragmentColor == sgNodeHandle_Invalid ) )
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
	if ( sgAddConnection ( Graph, Sampler, 0, Sample, 0 ) == false )
		{
		LOG_ERROR ( "Failed to connect Sampler to Sample" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, UV, 0, Sample, 1 ) == false )
		{
		LOG_ERROR ( "Failed to connect UV to Sample" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, Sample, 0, FragmentColor, 0 ) == false )
		{
		LOG_ERROR ( "Failed to connect Sample to FragmentColor" );
		TestResult = -1;
		goto End;
		}

	if ( sgGenerateGLSL ( Graph, sgGetDefaultGLSLGeneratorOptions(), &VertexShader, &FragmentShader ) == false )
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

	if ( ( strstr ( FragmentShader, "texture" ) == NULL ) || ( strstr ( FragmentShader, "uniform sampler2D AlbedoMap" ) == NULL ) ||
	        ( strstr ( FragmentShader, "in vec2 v_UV" ) == NULL ) )
		{
		LOG_ERROR ( "Unexpected fragment shader:\n%s\n", FragmentShader );
		TestResult = -1;
		goto End;
		}
	if ( ( strstr ( VertexShader, "out vec2 v_UV" ) == NULL ) || ( strstr ( VertexShader, "v_UV = UV" ) == NULL ) )
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
