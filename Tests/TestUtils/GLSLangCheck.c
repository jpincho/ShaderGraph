#include "GLSLangCheck.h"
#include <Platform/Logger.h>

#if defined ( GLSLANG_ENABLED )
#include <glslang/Include/glslang_c_interface.h>
#include <glslang/Public/resource_limits_c.h>
#include <string.h>

static bool InitializeGlslang ( void )
	{
	static bool Initialized = false;
	if ( Initialized )
		return true;
	if ( glslang_initialize_process() == 0 )
		{
		LOG_ERROR ( "Failed to initialize glslang" );
		return false;
		}
	Initialized = true;
	return true;
	}

static glslang_shader_t *CompileStage ( const glslang_stage_t Stage, const char *Code, const char *StageName )
	{
	glslang_input_t Input = {0};
	Input.language = GLSLANG_SOURCE_GLSL;
	Input.stage = Stage;
	Input.client = GLSLANG_CLIENT_OPENGL;
	Input.client_version = GLSLANG_TARGET_OPENGL_450;
	Input.target_language = GLSLANG_TARGET_NONE;
	Input.code = Code;
	Input.default_version = 330;
	Input.default_profile = GLSLANG_CORE_PROFILE;
	Input.messages = GLSLANG_MSG_DEFAULT_BIT;
	Input.resource = glslang_default_resource();

	glslang_shader_t *Shader = glslang_shader_create ( &Input );
	if ( Shader == NULL )
		{
		LOG_ERROR ( "Failed to create the %s shader", StageName );
		return NULL;
		}
	glslang_shader_set_options ( Shader, GLSLANG_SHADER_AUTO_MAP_LOCATIONS );
	if ( glslang_shader_preprocess ( Shader, &Input ) == 0 )
		{
		LOG_ERROR ( "%s shader preprocess failed:\n%s", StageName, glslang_shader_get_info_log ( Shader ) );
		goto OnError;
		}
	if ( glslang_shader_parse ( Shader, &Input ) == 0 )
		{
		LOG_ERROR ( "%s shader parse failed:\n%s", StageName, glslang_shader_get_info_log ( Shader ) );
		goto OnError;
		}
	return Shader;

OnError:
	LOG_DEBUG ( "%s shader:", StageName );
	LOG_DEBUG ( "%s", Code );
	glslang_shader_delete ( Shader );
	return NULL;
	}

bool sgTestGLSL ( const char *VertexShader, const char *FragmentShader )
	{
	if ( ( VertexShader == NULL ) || ( FragmentShader == NULL ) )
		{
		LOG_ERROR ( "Shader source is NULL" );
		return false;
		}
	if ( InitializeGlslang() == false )
		return false;

	glslang_shader_t *Vertex = CompileStage ( GLSLANG_STAGE_VERTEX, VertexShader, "vertex" );
	if ( Vertex == NULL )
		return false;
	glslang_shader_t *Fragment = CompileStage ( GLSLANG_STAGE_FRAGMENT, FragmentShader, "fragment" );
	if ( Fragment == NULL )
		{
		glslang_shader_delete ( Vertex );
		return false;
		}

	glslang_program_t *Program = glslang_program_create();
	if ( Program == NULL )
		{
		LOG_ERROR ( "Failed to create a glslang program" );
		glslang_shader_delete ( Vertex );
		glslang_shader_delete ( Fragment );
		return false;
		}
	glslang_program_add_shader ( Program, Vertex );
	glslang_program_add_shader ( Program, Fragment );
	const int Linked = glslang_program_link ( Program, GLSLANG_MSG_DEFAULT_BIT );
	if ( Linked == 0 )
		LOG_ERROR ( "Shader link failed:\n%s", glslang_program_get_info_log ( Program ) );

	glslang_program_delete ( Program );
	glslang_shader_delete ( Vertex );
	glslang_shader_delete ( Fragment );
	return Linked != 0;
	}
#else
bool sgTestGLSL ( const char *VertexShader, const char *FragmentShader )
	{
	( void ) VertexShader;
	( void ) FragmentShader;
	LOG_DEBUG ( "GLSLang disabled. No testing done" );
	return true;
	}
#endif
