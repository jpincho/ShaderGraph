R"(
struct AmbientLight
{
	vec3 Color;
	float Intensity;
};

struct PointLight
{
	vec4 Position;
	vec4 DiffuseColor;
	vec4 SpecularColor;
	float Intensity;
	float ConstantAttenuation;
	float LinearAttenuation;
	float QuadraticAttenuation;
};

struct SpotLight
{
	vec4 Position;
	vec4 Direction;
	vec4 DiffuseColor;
	vec4 SpecularColor;
	float Intensity;
	float ConstantAttenuation;
	float LinearAttenuation;
	float QuadraticAttenuation;
	float CosInnerCone;
	float CosOuterCone;
	float Pad0;
	float Pad1;
};

struct DirectionalLight
{
	vec4 Direction;
	vec4 DiffuseColor;
	vec4 SpecularColor;
	float Intensity;
	float Pad0;
	float Pad1;
	float Pad2;
};

layout ( std140 ) uniform LightBlock
{
	AmbientLight Ambient;
	PointLight Points[%u];
	SpotLight Spots[%u];
	DirectionalLight Directionals[%u];
	int PointCount;
	int SpotCount;
	int DirectionalCount;
} LightSources;
)"