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
	sgNodeHandle ColorAttr = sgAddAttribute ( Graph, "Color", sgValueType_Vec4 );
	sgNodeHandle VertexPosition = sgAddVertexPosition ( Graph );
	sgNodeHandle FragmentColor = sgAddFragmentColor ( Graph );
	if ( ( Position == sgNodeHandle_Invalid ) || ( ColorAttr == sgNodeHandle_Invalid ) ||
	        ( VertexPosition == sgNodeHandle_Invalid ) || ( FragmentColor == sgNodeHandle_Invalid ) )
		{
		LOG_ERROR ( "Failed to create nodes" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, Position, 0, VertexPosition, 0 ) == false )
		{
		LOG_ERROR ( "Position to VertexPosition failed" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, ColorAttr, 0, FragmentColor, 0 ) == false )
		{
		LOG_ERROR ( "ColorAttr to FragmentColor failed" );
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

	if ( ( strstr ( VertexShader, "out vec4 v_Color" ) == NULL ) || ( strstr ( VertexShader, "v_Color = Color" ) == NULL ) )
		{
		LOG_ERROR ( "Vertex shader missing color varying pass-through\n%s\n", VertexShader );
		TestResult = -1;
		goto End;
		}
	if ( strstr ( FragmentShader, "in vec4 v_Color" ) == NULL || strstr ( FragmentShader, "FragColor = v_Color" ) == NULL )
		{
		LOG_ERROR ( "Fragment shader missing color varying usage\n%s\n", FragmentShader );
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
