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

	sgNodeHandle Position = sgAddAttribute ( Graph, "Position", sgValueType_Vec4 );
	sgNodeHandle Normal = sgAddAttribute ( Graph, "Normal", sgValueType_Vec3 );
	sgNodeHandle Normalize = sgAddNormalize ( Graph );
	sgNodeHandle Compose = sgAddComposeVector ( Graph, sgValueType_Float, 4 );
	sgNodeHandle Nx = sgAddSwizzle ( Graph, "x", sgValueType_Vec3, sgValueType_Float );
	sgNodeHandle Ny = sgAddSwizzle ( Graph, "y", sgValueType_Vec3, sgValueType_Float );
	sgNodeHandle Nz = sgAddSwizzle ( Graph, "z", sgValueType_Vec3, sgValueType_Float );
	sgNodeHandle One = sgAddConstantFloat ( Graph, 1.0f );
	sgNodeHandle VertexPosition = sgAddVertexPosition ( Graph );
	sgNodeHandle FragmentColor = sgAddFragmentColor ( Graph );
	if ( ( Position == sgNodeHandle_Invalid ) || ( Normal == sgNodeHandle_Invalid ) || ( Normalize == sgNodeHandle_Invalid ) ||
	        ( Compose == sgNodeHandle_Invalid ) || ( Nx == sgNodeHandle_Invalid ) || ( Ny == sgNodeHandle_Invalid ) ||
	        ( Nz == sgNodeHandle_Invalid ) || ( One == sgNodeHandle_Invalid ) ||
	        ( VertexPosition == sgNodeHandle_Invalid ) || ( FragmentColor == sgNodeHandle_Invalid ) )
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
	if ( sgAddConnection ( Graph, Normal, 0, Normalize, 0 ) == false )
		{
		LOG_ERROR ( "Failed to connect Normal to Normalize" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, Normalize, 0, Nx, 0 ) == false )
		{
		LOG_ERROR ( "Failed to connect Normalize to Nx" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, Normalize, 0, Ny, 0 ) == false )
		{
		LOG_ERROR ( "Failed to connect Normalize to Ny" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, Normalize, 0, Nz, 0 ) == false )
		{
		LOG_ERROR ( "Failed to connect Normalize to Nz" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, Nx, 0, Compose, 0 ) == false )
		{
		LOG_ERROR ( "Failed to connect Nx to Compose" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, Ny, 0, Compose, 1 ) == false )
		{
		LOG_ERROR ( "Failed to connect Ny to Compose" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, Nz, 0, Compose, 2 ) == false )
		{
		LOG_ERROR ( "Failed to connect Nz to Compose" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, One, 0, Compose, 3 ) == false )
		{
		LOG_ERROR ( "Failed to connect One to Compose" );
		TestResult = -1;
		goto End;
		}
	if ( sgAddConnection ( Graph, Compose, 0, FragmentColor, 0 ) == false )
		{
		LOG_ERROR ( "Failed to connect Compose to FragmentColor" );
		TestResult = -1;
		goto End;
		}

	if ( sgGenerateGLSL ( Graph, &VertexShader, &FragmentShader ) == false )
		{
		LOG_ERROR ( "Failed to generate GLSL" );
		TestResult = -1;
		goto End;
		}
	if ( strstr ( FragmentShader, "normalize" ) == NULL )
		{
		LOG_ERROR ( "Fragment shader missing normalize:\n%s\n", FragmentShader );
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
