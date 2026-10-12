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

	sgGraphHandle Graph = sgCreate ();
	if ( Graph == sgGraphHandle_Invalid )
		{
		LOG_ERROR ( "Failed to create graph" );
		TestResult = -1;
		goto End;
		}

	sgNodeHandle Position = sgAddAttribute ( Graph, "Position", sgValueType_Vec4 );
	sgNodeHandle VertexPosition = sgAddVertexPosition ( Graph );
	sgNodeHandle Diffuse = sgAddConstantVec4 ( Graph, 1.0f, 0.2f, 0.2f, 1.0f );
	sgNodeHandle Specular = sgAddConstantVec4 ( Graph, 1.0f, 1.0f, 1.0f, 1.0f );
	sgNodeHandle Shininess = sgAddConstantFloat ( Graph, 32.0f );
	sgNodeHandle Normal = sgAddAttribute ( Graph, "Normal", sgValueType_Vec3 );
	sgNodeHandle WorldPosition = sgAddSwizzle ( Graph, "xyz", sgValueType_Vec4, sgValueType_Vec3 );
	sgNodeHandle CameraPosition = sgAddUniform ( Graph, "CameraPosition", sgValueType_Vec3 );
	sgNodeHandle Lit = sgAddBlinnPhongNode ( Graph );
	sgNodeHandle FragmentColor = sgAddFragmentColor ( Graph );

	if ( ( Position == sgNodeHandle_Invalid ) || ( VertexPosition == sgNodeHandle_Invalid ) ||
	        ( Diffuse == sgNodeHandle_Invalid ) || ( Specular == sgNodeHandle_Invalid ) ||
	        ( Shininess == sgNodeHandle_Invalid ) || ( Normal == sgNodeHandle_Invalid ) ||
	        ( WorldPosition == sgNodeHandle_Invalid ) || ( CameraPosition == sgNodeHandle_Invalid ) ||
	        ( Lit == sgNodeHandle_Invalid ) || ( FragmentColor == sgNodeHandle_Invalid ) )
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
	if ( sgAddConnection ( Graph, Position, 0, WorldPosition, 0 ) == false )
		{
		LOG_ERROR ( "Position to WorldPosition failed" );
		TestResult = -1;
		goto End;
		}
	if ( ( sgAddConnection ( Graph, Diffuse, 0, Lit, 0 ) == false ) ||
	        ( sgAddConnection ( Graph, Specular, 0, Lit, 1 ) == false ) ||
	        ( sgAddConnection ( Graph, Shininess, 0, Lit, 2 ) == false ) ||
	        ( sgAddConnection ( Graph, Normal, 0, Lit, 3 ) == false ) ||
	        ( sgAddConnection ( Graph, WorldPosition, 0, Lit, 4 ) == false ) ||
	        ( sgAddConnection ( Graph, CameraPosition, 0, Lit, 5 ) == false ) ||
	        ( sgAddConnection ( Graph, Lit, 0, FragmentColor, 0 ) == false ) )
		{
		LOG_ERROR ( "BlinnPhong wiring failed" );
		TestResult = -1;
		goto End;
		}

	if ( sgGenerateGLSL ( Graph, &VertexShader, &FragmentShader ) == false )
		{
		LOG_ERROR ( "GLSL generation failed" );
		TestResult = -1;
		goto End;
		}

	if ( sgTestGLSL ( VertexShader, FragmentShader ) == false )
		{
		LOG_ERROR ( "Shader failed to build" );
		TestResult = -1;
		goto End;
		}

	if ( strstr ( FragmentShader, "EvaluateLights" ) == NULL )
		{
		LOG_ERROR ( "Missing EvaluateLights call" );
		TestResult = -1;
		goto End;
		}
	if ( strstr ( FragmentShader, "uniform LightBlock" ) == NULL )
		{
		LOG_ERROR ( "Missing LightBlock UBO" );
		TestResult = -1;
		goto End;
		}
	if ( ( strstr ( FragmentShader, "EvaluatePointLight" ) == NULL ) ||
	        ( strstr ( FragmentShader, "EvaluateSpotLight" ) == NULL ) ||
	        ( strstr ( FragmentShader, "EvaluateDirectionalLight" ) == NULL ) )
		{
		LOG_ERROR ( "Missing per-light evaluators" );
		TestResult = -1;
		goto End;
		}
	if ( strstr ( FragmentShader, "AmbientLight" ) == NULL )
		{
		LOG_ERROR ( "Missing ambient light support" );
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
