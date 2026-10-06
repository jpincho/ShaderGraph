#include "ShaderCreator.h"
#include <Platform/Logger.h>

enum
	{
	sgSkinningOut_Position = 0,
	sgSkinningOut_Normal = 1
	};

typedef struct
	{
	sgNodeHandle Node;
	unsigned Output;
	} Pin;

static Pin MakePin ( const sgNodeHandle Node, const unsigned Output )
	{
	Pin Result;
	Result.Node = Node;
	Result.Output = Output;
	return Result;
	}

static Pin Out0 ( const sgNodeHandle Node )
	{
	return MakePin ( Node, 0 );
	}

static bool Connect ( const sgGraphHandle Graph, const Pin From, const sgNodeHandle To, const unsigned ToPin )
	{
	if ( ( From.Node == sgNodeHandle_Invalid ) || ( To == sgNodeHandle_Invalid ) )
		return false;
	return sgAddConnection ( Graph, From.Node, From.Output, To, ToPin );
	}

static bool UsesTexture ( const scMaterialSource Source )
	{
	return Source == scMaterialSource_Texture;
	}

static bool UsesColorAttribute ( const scMaterialSource Source )
	{
	return Source == scMaterialSource_Attribute;
	}

static sgNodeHandle Multiply ( const sgGraphHandle Graph, const Pin A, const Pin B,
                               const sgValueType AType, const sgValueType BType )
	{
	sgNodeHandle Node = sgAddMultiplyTyped ( Graph, AType, BType );
	if ( Node == sgNodeHandle_Invalid )
		return sgNodeHandle_Invalid;
	if ( ( Connect ( Graph, A, Node, 0 ) == false ) || ( Connect ( Graph, B, Node, 1 ) == false ) )
		return sgNodeHandle_Invalid;
	return Node;
	}

static sgNodeHandle SampleTexture ( const sgGraphHandle Graph, const Pin Sampler, const Pin UV )
	{
	sgNodeHandle Node = sgAddTextureSample ( Graph );
	if ( Node == sgNodeHandle_Invalid )
		return sgNodeHandle_Invalid;
	if ( ( Connect ( Graph, Sampler, Node, 0 ) == false ) || ( Connect ( Graph, UV, Node, 1 ) == false ) )
		return sgNodeHandle_Invalid;
	return Node;
	}

static sgNodeHandle Swizzle ( const sgGraphHandle Graph, const Pin Value, const char *Mask,
                              const sgValueType InputType, const sgValueType OutputType )
	{
	sgNodeHandle Node = sgAddSwizzle ( Graph, Mask, InputType, OutputType );
	if ( Node == sgNodeHandle_Invalid )
		return sgNodeHandle_Invalid;
	if ( Connect ( Graph, Value, Node, 0 ) == false )
		return sgNodeHandle_Invalid;
	return Node;
	}

static sgNodeHandle ComposeVec4FromVec3 ( const sgGraphHandle Graph, const Pin XYZ, const Pin W )
	{
	sgNodeHandle X = Swizzle ( Graph, XYZ, "x", sgValueType_Vec3, sgValueType_Float );
	sgNodeHandle Y = Swizzle ( Graph, XYZ, "y", sgValueType_Vec3, sgValueType_Float );
	sgNodeHandle Z = Swizzle ( Graph, XYZ, "z", sgValueType_Vec3, sgValueType_Float );
	sgNodeHandle Compose = sgAddComposeVec4 ( Graph );
	if ( ( X == sgNodeHandle_Invalid ) || ( Y == sgNodeHandle_Invalid ) || ( Z == sgNodeHandle_Invalid ) ||
	        ( Compose == sgNodeHandle_Invalid ) )
		return sgNodeHandle_Invalid;
	if ( ( Connect ( Graph, Out0 ( X ), Compose, 0 ) == false ) ||
	        ( Connect ( Graph, Out0 ( Y ), Compose, 1 ) == false ) ||
	        ( Connect ( Graph, Out0 ( Z ), Compose, 2 ) == false ) ||
	        ( Connect ( Graph, W, Compose, 3 ) == false ) )
		return sgNodeHandle_Invalid;
	return Compose;
	}

static bool ResolveVec4Source ( const sgGraphHandle Graph, const scMaterialSource Source,
                                const char *UniformName, const char *TextureName,
                                const sgNodeHandle UV, const sgNodeHandle ColorAttr,
                                const Pin OffValue, Pin *Out )
	{
	switch ( Source )
		{
		case scMaterialSource_Uniform:
			{
			sgNodeHandle Uniform = sgAddUniform ( Graph, UniformName, sgValueType_Vec4 );
			if ( Uniform == sgNodeHandle_Invalid )
				return false;
			*Out = Out0 ( Uniform );
			return true;
			}
		case scMaterialSource_Attribute:
			if ( ColorAttr == sgNodeHandle_Invalid )
				return false;
			*Out = Out0 ( ColorAttr );
			return true;
		case scMaterialSource_Texture:
			{
			sgNodeHandle Sampler = sgAddUniform ( Graph, TextureName, sgValueType_Sampler2D );
			sgNodeHandle Sample = SampleTexture ( Graph, Out0 ( Sampler ), Out0 ( UV ) );
			if ( ( Sampler == sgNodeHandle_Invalid ) || ( Sample == sgNodeHandle_Invalid ) )
				return false;
			*Out = Out0 ( Sample );
			return true;
			}
		case scMaterialSource_Off:
		default:
			*Out = OffValue;
			return true;
		}
	}

static bool ResolveFloatSource ( const sgGraphHandle Graph, const scMaterialSource Source,
                                 const char *UniformName, const char *TextureName,
                                 const sgNodeHandle UV, const sgNodeHandle FloatAttr,
                                 const Pin OffValue, Pin *Out )
	{
	switch ( Source )
		{
		case scMaterialSource_Uniform:
			{
			sgNodeHandle Uniform = sgAddUniform ( Graph, UniformName, sgValueType_Float );
			if ( Uniform == sgNodeHandle_Invalid )
				return false;
			*Out = Out0 ( Uniform );
			return true;
			}
		case scMaterialSource_Attribute:
			if ( FloatAttr == sgNodeHandle_Invalid )
				return false;
			*Out = Out0 ( FloatAttr );
			return true;
		case scMaterialSource_Texture:
			{
			sgNodeHandle Sampler = sgAddUniform ( Graph, TextureName, sgValueType_Sampler2D );
			sgNodeHandle Sample = SampleTexture ( Graph, Out0 ( Sampler ), Out0 ( UV ) );
			sgNodeHandle Channel = Swizzle ( Graph, Out0 ( Sample ), "a", sgValueType_Vec4, sgValueType_Float );
			if ( ( Sampler == sgNodeHandle_Invalid ) || ( Sample == sgNodeHandle_Invalid ) ||
			        ( Channel == sgNodeHandle_Invalid ) )
				return false;
			*Out = Out0 ( Channel );
			return true;
			}
		case scMaterialSource_Off:
		default:
			*Out = OffValue;
			return true;
		}
	}

static bool ResolveNormal ( const sgGraphHandle Graph, const scMaterialSource Source,
                            const Pin LocalNormal, const sgNodeHandle Model,
                            const sgNodeHandle UV, Pin *OutNormal )
	{
	if ( Source == scMaterialSource_Texture )
		{
		sgNodeHandle Tangent = sgAddAttribute ( Graph, "Tangent", sgValueType_Vec3 );
		sgNodeHandle Bitangent = sgAddAttribute ( Graph, "Bitangent", sgValueType_Vec3 );
		sgNodeHandle TBN = sgAddTBN ( Graph );
		sgNodeHandle Sampler = sgAddUniform ( Graph, "NormalMap", sgValueType_Sampler2D );
		sgNodeHandle Sample = SampleTexture ( Graph, Out0 ( Sampler ), Out0 ( UV ) );
		sgNodeHandle SampleRGB = Swizzle ( Graph, Out0 ( Sample ), "rgb", sgValueType_Vec4, sgValueType_Vec3 );
		sgNodeHandle Mapped = sgAddNormalMap ( Graph );
		if ( ( Tangent == sgNodeHandle_Invalid ) || ( Bitangent == sgNodeHandle_Invalid ) ||
		        ( TBN == sgNodeHandle_Invalid ) || ( Sampler == sgNodeHandle_Invalid ) ||
		        ( Sample == sgNodeHandle_Invalid ) || ( SampleRGB == sgNodeHandle_Invalid ) ||
		        ( Mapped == sgNodeHandle_Invalid ) )
			return false;
		/* TBN pins: Normal, Tangent, Bitangent */
		if ( ( Connect ( Graph, LocalNormal, TBN, 0 ) == false ) ||
		        ( Connect ( Graph, Out0 ( Tangent ), TBN, 1 ) == false ) ||
		        ( Connect ( Graph, Out0 ( Bitangent ), TBN, 2 ) == false ) )
			return false;
		/* NormalMap pins: TBN, Sample */
		if ( ( Connect ( Graph, Out0 ( TBN ), Mapped, 0 ) == false ) ||
		        ( Connect ( Graph, Out0 ( SampleRGB ), Mapped, 1 ) == false ) )
			return false;
		*OutNormal = Out0 ( Mapped );
		return true;
		}

	/* Attribute or other non-texture: transform local normal by model matrix. */
	sgNodeHandle Model3 = sgAddMat3Cast ( Graph );
	sgNodeHandle Transformed = Multiply ( Graph, Out0 ( Model3 ), LocalNormal, sgValueType_Mat3, sgValueType_Vec3 );
	sgNodeHandle Normalized = sgAddNormalize ( Graph );
	if ( ( Model3 == sgNodeHandle_Invalid ) || ( Transformed == sgNodeHandle_Invalid ) ||
	        ( Normalized == sgNodeHandle_Invalid ) )
		return false;
	if ( ( Connect ( Graph, Out0 ( Model ), Model3, 0 ) == false ) ||
	        ( Connect ( Graph, Out0 ( Transformed ), Normalized, 0 ) == false ) )
		return false;
	*OutNormal = Out0 ( Normalized );
	return true;
	}

sgGraphHandle sgCreateGraphFromDefinition ( const scShaderDefinition Definition )
	{
	if ( Definition.Skinning && ( Definition.BoneCount == 0 ) )
		{
		LOG_ERROR ( "Skinning requires a non-zero BoneCount" );
		return sgGraphHandle_Invalid;
		}

	sgGraphHandle Graph = sgCreate ();
	if ( Graph == sgGraphHandle_Invalid )
		return sgGraphHandle_Invalid;

	const bool NeedTextures = UsesTexture ( Definition.DiffuseSource ) ||
	                          UsesTexture ( Definition.SpecularSource ) ||
	                          UsesTexture ( Definition.ShininessSource ) ||
	                          UsesTexture ( Definition.EmissivenessSource ) ||
	                          UsesTexture ( Definition.NormalSource );
	const bool NeedColorAttr = UsesColorAttribute ( Definition.DiffuseSource ) ||
	                           UsesColorAttribute ( Definition.SpecularSource ) ||
	                           UsesColorAttribute ( Definition.EmissivenessSource );
	const bool UseLighting = ( Definition.LightModel == scLightModel_BlinnPhong ) &&
	                         ( Definition.NormalSource != scMaterialSource_Off );

	sgNodeHandle Position = sgAddAttribute ( Graph, "Position", sgValueType_Vec3 );
	sgNodeHandle Model = sgAddUniform ( Graph, "ModelMatrix", sgValueType_Mat4 );
	sgNodeHandle View = sgAddUniform ( Graph, "ViewMatrix", sgValueType_Mat4 );
	sgNodeHandle Projection = sgAddUniform ( Graph, "ProjectionMatrix", sgValueType_Mat4 );
	if ( ( Position == sgNodeHandle_Invalid ) || ( Model == sgNodeHandle_Invalid ) ||
	        ( View == sgNodeHandle_Invalid ) || ( Projection == sgNodeHandle_Invalid ) )
		goto OnError;

	Pin LocalPosition;
	Pin LocalNormal = MakePin ( sgNodeHandle_Invalid, 0 );

	if ( Definition.Skinning || UseLighting )
		{
		sgNodeHandle NormalAttr = sgAddAttribute ( Graph, "Normal", sgValueType_Vec3 );
		if ( NormalAttr == sgNodeHandle_Invalid )
			goto OnError;
		LocalNormal = Out0 ( NormalAttr );
		}

	if ( Definition.Skinning )
		{
		sgNodeHandle BoneIndex = sgAddAttribute ( Graph, "BoneIndex", sgValueType_UVec4 );
		sgNodeHandle BoneWeight = sgAddAttribute ( Graph, "BoneWeight", sgValueType_Vec4 );
		sgNodeHandle Skin = sgAddSkinning ( Graph );
		sgSetSkinningBoneCount ( Graph, Skin, Definition.BoneCount );
		if ( ( BoneIndex == sgNodeHandle_Invalid ) || ( BoneWeight == sgNodeHandle_Invalid ) ||
		        ( Skin == sgNodeHandle_Invalid ) )
			goto OnError;
		if ( ( Connect ( Graph, Out0 ( Position ), Skin, 0 ) == false ) ||
		        ( Connect ( Graph, LocalNormal, Skin, 1 ) == false ) ||
		        ( Connect ( Graph, Out0 ( BoneIndex ), Skin, 2 ) == false ) ||
		        ( Connect ( Graph, Out0 ( BoneWeight ), Skin, 3 ) == false ) )
			goto OnError;
		LocalPosition = MakePin ( Skin, sgSkinningOut_Position );
		LocalNormal = MakePin ( Skin, sgSkinningOut_Normal );
		}
	else
		{
		sgNodeHandle One = sgAddConstantFloat ( Graph, 1.0f );
		sgNodeHandle Local = ComposeVec4FromVec3 ( Graph, Out0 ( Position ), Out0 ( One ) );
		if ( ( One == sgNodeHandle_Invalid ) || ( Local == sgNodeHandle_Invalid ) )
			goto OnError;
		LocalPosition = Out0 ( Local );
		}

	/* Skinning outputs vec3 position; compose to vec4 for the transform chain. */
	if ( Definition.Skinning )
		{
		sgNodeHandle One = sgAddConstantFloat ( Graph, 1.0f );
		sgNodeHandle Local4 = ComposeVec4FromVec3 ( Graph, LocalPosition, Out0 ( One ) );
		if ( ( One == sgNodeHandle_Invalid ) || ( Local4 == sgNodeHandle_Invalid ) )
			goto OnError;
		LocalPosition = Out0 ( Local4 );
		}

	sgNodeHandle World = Multiply ( Graph, Out0 ( Model ), LocalPosition, sgValueType_Mat4, sgValueType_Vec4 );
	sgNodeHandle Eye = Multiply ( Graph, Out0 ( View ), Out0 ( World ), sgValueType_Mat4, sgValueType_Vec4 );
	sgNodeHandle Clip = Multiply ( Graph, Out0 ( Projection ), Out0 ( Eye ), sgValueType_Mat4, sgValueType_Vec4 );
	sgNodeHandle VertexPosition = sgAddVertexPosition ( Graph );
	if ( ( World == sgNodeHandle_Invalid ) || ( Eye == sgNodeHandle_Invalid ) ||
	        ( Clip == sgNodeHandle_Invalid ) || ( VertexPosition == sgNodeHandle_Invalid ) )
		goto OnError;
	if ( Connect ( Graph, Out0 ( Clip ), VertexPosition, 0 ) == false )
		goto OnError;

	sgNodeHandle UV = sgNodeHandle_Invalid;
	if ( NeedTextures )
		{
		UV = sgAddAttribute ( Graph, "UV", sgValueType_Vec2 );
		if ( UV == sgNodeHandle_Invalid )
			goto OnError;
		}

	sgNodeHandle ColorAttr = sgNodeHandle_Invalid;
	if ( NeedColorAttr )
		{
		ColorAttr = sgAddAttribute ( Graph, "Color", sgValueType_Vec4 );
		if ( ColorAttr == sgNodeHandle_Invalid )
			goto OnError;
		}

	sgNodeHandle White = sgAddConstantVec4 ( Graph, 1.0f, 1.0f, 1.0f, 1.0f );
	if ( White == sgNodeHandle_Invalid )
		goto OnError;

	Pin Diffuse;
	if ( ResolveVec4Source ( Graph, Definition.DiffuseSource, "Diffuse", "DiffuseMap",
	                         UV, ColorAttr, Out0 ( White ), &Diffuse ) == false )
		goto OnError;

	Pin FinalColor = Diffuse;

	if ( UseLighting )
		{
		Pin Normal;
		if ( ResolveNormal ( Graph, Definition.NormalSource, LocalNormal, Model, UV, &Normal ) == false )
			goto OnError;

		sgNodeHandle ShininessAttr = sgNodeHandle_Invalid;
		if ( Definition.ShininessSource == scMaterialSource_Attribute )
			{
			ShininessAttr = sgAddAttribute ( Graph, "Shininess", sgValueType_Float );
			if ( ShininessAttr == sgNodeHandle_Invalid )
				goto OnError;
			}
		sgNodeHandle DefaultShininess = sgAddConstantFloat ( Graph, 32.0f );
		if ( DefaultShininess == sgNodeHandle_Invalid )
			goto OnError;
		Pin Shininess;
		if ( ResolveFloatSource ( Graph, Definition.ShininessSource, "Shininess", "ShininessMap",
		                          UV, ShininessAttr, Out0 ( DefaultShininess ), &Shininess ) == false )
			goto OnError;

		sgNodeHandle WorldPosition = Swizzle ( Graph, Out0 ( World ), "xyz", sgValueType_Vec4, sgValueType_Vec3 );
		sgNodeHandle CameraPosition = sgAddUniform ( Graph, "CameraPosition", sgValueType_Vec3 );
		sgNodeHandle Lit = sgAddBlinnPhong ( Graph );
		if ( ( WorldPosition == sgNodeHandle_Invalid ) || ( CameraPosition == sgNodeHandle_Invalid ) ||
		        ( Lit == sgNodeHandle_Invalid ) )
			goto OnError;

		Pin Specular;
		if ( ResolveVec4Source ( Graph, Definition.SpecularSource, "Specular", "SpecularMap",
		                         UV, ColorAttr, Out0 ( White ), &Specular ) == false )
			goto OnError;

		/* BlinnPhong: Diffuse, Specular, Shininess, Normal, WorldPosition, CameraPosition */
		if ( ( Connect ( Graph, Diffuse, Lit, 0 ) == false ) ||
		        ( Connect ( Graph, Specular, Lit, 1 ) == false ) ||
		        ( Connect ( Graph, Shininess, Lit, 2 ) == false ) ||
		        ( Connect ( Graph, Normal, Lit, 3 ) == false ) ||
		        ( Connect ( Graph, Out0 ( WorldPosition ), Lit, 4 ) == false ) ||
		        ( Connect ( Graph, Out0 ( CameraPosition ), Lit, 5 ) == false ) )
			goto OnError;
		FinalColor = Out0 ( Lit );
		}

	if ( Definition.EmissivenessSource != scMaterialSource_Off )
		{
		sgNodeHandle EmissiveAttr = sgNodeHandle_Invalid;
		if ( Definition.EmissivenessSource == scMaterialSource_Attribute )
			{
			EmissiveAttr = sgAddAttribute ( Graph, "Emissive", sgValueType_Vec4 );
			if ( EmissiveAttr == sgNodeHandle_Invalid )
				goto OnError;
			}
		sgNodeHandle Black = sgAddConstantVec4 ( Graph, 0.0f, 0.0f, 0.0f, 0.0f );
		if ( Black == sgNodeHandle_Invalid )
			goto OnError;
		Pin Emissive;
		if ( ResolveVec4Source ( Graph, Definition.EmissivenessSource, "Emissive", "EmissiveMap",
		                         UV, EmissiveAttr, Out0 ( Black ), &Emissive ) == false )
			goto OnError;
		sgNodeHandle Sum = sgAddAddition ( Graph );
		if ( Sum == sgNodeHandle_Invalid )
			goto OnError;
		if ( ( Connect ( Graph, FinalColor, Sum, 0 ) == false ) ||
		        ( Connect ( Graph, Emissive, Sum, 1 ) == false ) )
			goto OnError;
		FinalColor = Out0 ( Sum );
		}

	sgNodeHandle FragmentColor = sgAddFragmentColor ( Graph );
	if ( FragmentColor == sgNodeHandle_Invalid )
		goto OnError;
	if ( Connect ( Graph, FinalColor, FragmentColor, 0 ) == false )
		goto OnError;

	return Graph;

OnError:
	sgDestroy ( Graph );
	return sgGraphHandle_Invalid;
	}
