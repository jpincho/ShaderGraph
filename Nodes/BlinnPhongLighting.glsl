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

vec3 EvaluateBlinnPhong ( vec3 LightVector, vec3 ViewVector, vec3 Normal,
	vec3 LightDiffuse, vec3 LightSpecular, float LightIntensity,
	vec3 MaterialDiffuse, vec3 MaterialSpecular, float Shininess )
{
	vec3 L = normalize ( LightVector );
	vec3 N = normalize ( Normal );
	vec3 V = normalize ( ViewVector );
	vec3 H = normalize ( L + V );
	float DiffuseFactor = max ( dot ( N, L ), 0.0 );
	float SpecularFactor = pow ( max ( dot ( N, H ), 0.0 ), max ( Shininess, 1.0 ) );
	return ( DiffuseFactor * LightDiffuse * MaterialDiffuse
		+ SpecularFactor * LightSpecular * MaterialSpecular ) * LightIntensity;
}

float EvaluateAttenuation ( float Distance, float ConstantTerm, float LinearTerm, float QuadraticTerm )
{
	return ConstantTerm + LinearTerm * Distance + QuadraticTerm * Distance * Distance;
}

vec3 EvaluatePointLight ( int LightIndex, vec3 WorldPosition, vec3 ViewVector, vec3 Normal,
	vec3 MaterialDiffuse, vec3 MaterialSpecular, float Shininess )
{
	PointLight Light = LightSources.Points[LightIndex];
	vec3 ToLight = Light.Position.xyz - WorldPosition;
	float Distance = length ( ToLight );
	float Attenuation = EvaluateAttenuation ( Distance, Light.ConstantAttenuation,
		Light.LinearAttenuation, Light.QuadraticAttenuation );
	vec3 Lit = EvaluateBlinnPhong ( ToLight, ViewVector, Normal,
		Light.DiffuseColor.rgb, Light.SpecularColor.rgb, Light.Intensity,
		MaterialDiffuse, MaterialSpecular, Shininess );
	return Lit / max ( Attenuation, 1e-4 );
}

vec3 EvaluateSpotLight ( int LightIndex, vec3 WorldPosition, vec3 ViewVector, vec3 Normal,
	vec3 MaterialDiffuse, vec3 MaterialSpecular, float Shininess )
{
	SpotLight Light = LightSources.Spots[LightIndex];
	vec3 ToLight = Light.Position.xyz - WorldPosition;
	float Distance = length ( ToLight );
	vec3 LightDir = normalize ( ToLight );
	float Cone = dot ( LightDir, normalize ( -Light.Direction.xyz ) );
	float ConeRange = Light.CosInnerCone - Light.CosOuterCone;
	float ConeFactor = clamp ( ( Cone - Light.CosOuterCone ) / max ( ConeRange, 1e-4 ), 0.0, 1.0 );
	if ( ConeFactor <= 0.0 )
		return vec3 ( 0.0 );
	float Attenuation = EvaluateAttenuation ( Distance, Light.ConstantAttenuation,
		Light.LinearAttenuation, Light.QuadraticAttenuation );
	vec3 Lit = EvaluateBlinnPhong ( ToLight, ViewVector, Normal,
		Light.DiffuseColor.rgb, Light.SpecularColor.rgb, Light.Intensity,
		MaterialDiffuse, MaterialSpecular, Shininess );
	return ( Lit / max ( Attenuation, 1e-4 ) ) * ConeFactor;
}

vec3 EvaluateDirectionalLight ( int LightIndex, vec3 ViewVector, vec3 Normal,
	vec3 MaterialDiffuse, vec3 MaterialSpecular, float Shininess )
{
	DirectionalLight Light = LightSources.Directionals[LightIndex];
	return EvaluateBlinnPhong ( -Light.Direction.xyz, ViewVector, Normal,
		Light.DiffuseColor.rgb, Light.SpecularColor.rgb, Light.Intensity,
		MaterialDiffuse, MaterialSpecular, Shininess );
}

vec4 EvaluateLights ( vec4 MaterialDiffuse, vec4 MaterialSpecular, float Shininess,
	vec3 Normal, vec3 WorldPosition, vec3 CameraPosition )
{
	vec3 ViewVector = CameraPosition - WorldPosition;
	vec3 Color = LightSources.Ambient.Color * LightSources.Ambient.Intensity * MaterialDiffuse.rgb;
	for ( int LightIndex = 0; LightIndex < LightSources.PointCount; ++LightIndex )
		Color += EvaluatePointLight ( LightIndex, WorldPosition, ViewVector, Normal,
			MaterialDiffuse.rgb, MaterialSpecular.rgb, Shininess );
	for ( int LightIndex = 0; LightIndex < LightSources.SpotCount; ++LightIndex )
		Color += EvaluateSpotLight ( LightIndex, WorldPosition, ViewVector, Normal,
			MaterialDiffuse.rgb, MaterialSpecular.rgb, Shininess );
	for ( int LightIndex = 0; LightIndex < LightSources.DirectionalCount; ++LightIndex )
		Color += EvaluateDirectionalLight ( LightIndex, ViewVector, Normal,
			MaterialDiffuse.rgb, MaterialSpecular.rgb, Shininess );
	return vec4 ( Color, MaterialDiffuse.a );
}

)"