#ifndef _DEFAULT_SHADERS_
#define _DEFAULT_SHADERS_



#include "GL3W\include\glew.h"

#pragma comment (lib, "GL3W/libx86/glew32.lib") 
typedef unsigned int uint;

static const GLchar* vertexShaderSource[] =
{
	"#version 330 core\n"
	"layout(location = 0) in vec3 position;\n"
	"layout(location = 1) in vec2 texCoord;\n"
	"layout(location = 2) in vec3 normal;\n"
	"layout(location = 3) in vec4 color; \n"
	"\n"
	"\n"
	"\n"
	"out float ourTime;\n"
	"out vec4 ourColor;\n"
	"out vec3 ourNormal;\n"
	"out vec2 TexCoord;\n"
	"out vec3 ourPos;\n"
	"uniform float _time;\n"
	"uniform vec4 _color;\n"
	"uniform mat4 model;\n"
	"uniform mat4 viewproj;\n"
	"uniform mat4 view;\n"
	"\n"
	"uniform sampler2D height_map;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	TexCoord = texCoord;\n"
	"	ourTime = _time;\n"
	"	ourPos = position;\n"
	"	ourNormal = normal;\n"
	"\n"
	"	gl_Position = viewproj *  model * vec4(position, 1.0f);\n"
	"}\n"
	
};

static const GLchar* fragmentShaderSource[] =
{
"#version 330 core \n"
"\n"
"\n"
"#define MAX_LIGHTS 30\n"
"	uniform int _numLights;\n"
"uniform struct Light {\n"
"	vec3 position;\n"
"	int type;\n"
"	vec4 l_color; //a.k.a the color of the light\n"
"	float intensity; \n"
"	float ambientCoefficient; \n"
"	float radius;\n"
"\n"
"} _lights[MAX_LIGHTS];\n"
"\n"
"in vec4 ourColor;\n"
"in vec2 TexCoord;\n"
"in vec3 ourNormal;\n"
"in vec3 ourPos;\n"
"\n"
"uniform vec4 diff_color;							 \n"
"out vec4 color;									 \n"
"uniform sampler2D albedo;							 \n"
"uniform sampler2D normal_map;						 \n"
"uniform sampler2D occlusion_map;					 \n"
"													 \n"
"													 \n"
"uniform mat4 viewproj;								 \n"
"uniform mat4 model;								 \n"
"uniform mat4 view;									 \n"
"uniform vec3 _cameraPosition;						 \n"
"													 \n"
"uniform float a_LightInt;							 \n"
"uniform float a_Ka;								 \n"
"uniform float a_Kd;								 \n"
"uniform float a_Ks;								 \n"
"uniform float a_shininess;							 \n"
"													 \n"
"													 \n"
"vec3 blinnPhongDir(Light light, float lightInt, float Ka, float Kd, float Ks, float shininess, vec3 N)					 \n"
"{																												 \n"
"																												 \n"
"																												 \n"

"																												 \n"
"	vec3 surfacePos = vec3(model * vec4(ourPos, 1));															 \n"
"	vec3 surfaceToCamera = normalize(_cameraPosition - surfacePos);												 \n"
"	vec3 lightDir;																								 \n"
"																												 \n"
"	vec3 normal = normalize(transpose(inverse(mat3(model)))*ourNormal * N);										 \n"
"																												 \n"
"	if (light.type != 0) {																						 \n"
"																												 \n"
"																												 \n"
"		lightDir = normalize(light.position);																	 \n"
"																												 \n"
"																												 \n"
"		vec3 s = normalize(lightDir);																			 \n"
"																												 \n"
"		float diffuse = Kd * lightInt *  max(dot(normal ,s),0);											 \n"
"		float spec = 0;																							 \n"
"																												 \n"
"		spec = Ks* pow(max(0, dot(surfaceToCamera,reflect(-s,normal))), shininess);								 \n"
"		return vec3(diffuse, spec,1);																			 \n"
"																												 \n"
"	}																											 \n"
"																												 \n"
"	else {																										 \n"
"																												 \n"
"																												 \n"
"																												 \n"
"		vec3 s = normalize(light.position - surfacePos);														 \n"
"																												 \n"
"		float d = length((light.position - surfacePos)/light.radius);										 \n"
"		 float attenuation =  ((d * d) / (light.radius * light.radius));													 \n"
"																												 \n"
"		float diffuse = attenuation * Kd *lightInt * max(0, dot(normal , s));								 \n"
"		float spec = 0;																							 \n"
"		if (diffuse >0)																							 \n"
"			spec = attenuation * Ks *lightInt* pow(max(0, dot(surfaceToCamera, reflect(-s, normal))), shininess);\n"
"																												 \n"
"		return vec3(diffuse,spec,attenuation);																	 \n"
"																												 \n"
"																												 \n"
"	}																											 \n"
"																												 \n"
"																												 \n"
"}																												 \n"
"																												 \n"
"																												 \n"
"void main()																									 \n"
"{																												 \n"
"	// light data																								 \n"
"	vec3 lightdir = normalize(vec3(1,1, 0));																	 \n"
"																												 \n"
"	vec3 lcolor = diff_color.xyz;																				 \n"
"	vec3 viewDirection = normalize(vec3(view *vec4(1,1,1, 1.0) - model * vec4(ourPos,1)));						 \n"
"																												 \n"
"																												 \n"
"	vec3 color_texture = texture(albedo, TexCoord).xyz;															 \n"
"	vec3 N = normalize(texture(normal_map,TexCoord).xyz);														 \n"
"	vec3 occlusion_texture = texture(occlusion_map,TexCoord).xyz;												 \n"

"	vec3 normal = normalize(ourNormal);// + normal_map.x * vec3(1,0,0) + normal_map.y* vec3(0,1,0); 			 \n"
"	vec3 specularReflection;																					 \n"
"																												 \n"
"	vec3 inten = vec3(0);																						 \n"
"	vec3 inten_final = vec3(0);																					 \n"
"	vec3 tot_light_ilu_color = vec3(0);																			 \n"
"	vec4 light_colors[MAX_LIGHTS];																				 \n"
"	for (int i = 0; i <_numLights; ++i) {																		 \n"
"		inten = blinnPhongDir(_lights[i], _lights[i].intensity, _lights[i].ambientCoefficient, a_Kd, a_Ks, a_shininess, N);						\n"
"		inten_final.xy += inten.xy;																				 \n"
"		light_colors[i] = vec4(_lights[i].l_color.rgb,inten.z);													 \n"
"	}																											 \n"
"	float final_ambient = 0;																											 \n"
"	vec3 final_color = vec3(0);																					 \n"
"	for (int i = 0; i<_numLights; ++i) {																		 \n"
"		final_color += vec3(light_colors[i]) * light_colors[i].a;												 \n"
"		final_ambient += _lights[i].ambientCoefficient;\n"
"	}																											 \n"
"	final_ambient = final_ambient/_numLights;\n"
"	final_color = normalize(final_color);																		 \n"
"																												 \n"
"	color = vec4(final_ambient* color_texture +vec3(0,0.03,0.07) + color_texture * (inten_final.x + inten_final.y)*occlusion_texture*final_color.rgb , 1);		 \n"
"																												 \n"
"}																												 \n"
};

static const GLchar* DefaultVert[] =
{
	"#version 330 core\n"
	"layout(location = 0) in vec3 position;\n"
	"layout(location = 1) in vec2 texCoord;\n"
	"layout(location = 2) in vec3 normal;\n"
	"layout(location = 3) in vec4 color;\n"
	"out float ourTime;\n"
	"out vec4 ourColor;\n"
	"out vec3 ourNormal;\n"
	"out vec2 TexCoord;\n"
	"uniform float _time;\n"
	"uniform vec4 _color;\n"
	"uniform mat4 model;\n"
	"uniform mat4 viewproj;\n"
	"uniform mat4 view;\n"
	"void main()\n"
	"{\n"
	"gl_Position = viewproj *  model * vec4(position.x,position.y,position.z, 1.0f);\n"
	"ourColor = _color;\n"
	"TexCoord = texCoord;\n"
	"ourTime = _time;\n"
	"ourNormal = mat3(model) * normal;"
	"}\n"

};

static const GLchar* TextureVert[] =
{
	"#version 330 core\n"
	"layout(location = 0) in vec3 position;\n"
	"layout(location = 1) in vec2 texCoord;\n"


	"out vec2 TexCoord;\n"

	"void main()\n"
	"{\n"
	"gl_Position = vec4(position, 1.0f);\n"	
	"TexCoord = texCoord;\n"


	"}\n"
};

static const GLchar* BlurFrag[] =
{
	"#version 330 core\n"

	"in vec2 TexCoord;\n"
	"out vec4 color;\n"
	"uniform sampler2D albedo;\n"
	"uniform int _orientation;\n"
	"float Gaussian(float x, float deviation)\n"
	"{\n"
	"	return (1.0 / sqrt(2.0 * 3.141592 * deviation)) * exp(-((x * x) / (2.0 * deviation)));\n"
	"}\n"

	"void main()\n"
	"{\n"

	"float halfBlur = float(10) * 0.5;\n"
	"vec4 colour = vec4(0.0);\n"
	"vec4 texColour = vec4(0.0);\n"


	"float deviation = halfBlur * 0.35;\n"
	"deviation *= deviation;\n"
	"float strength = 1.0 - 0.5;\n"

	"if(_orientation == 0){\n"
	"	// Horizontal blur\n"
	"	for (int i = 0; i < 10; ++i)\n"
	"	{\n"
	"		if (i >= 10)\n"
	"			break;\n"
	"\n"
	"		float offset = float(i) - halfBlur;\n"
	"		texColour = texture2D(albedo, TexCoord + vec2(offset / 64, 0)) * Gaussian(offset * strength, deviation);\n"
	"		colour += texColour;\n"
	"	}\n"
	"}\n"
	"else {\n"
	"	// Horizontal blur\n"
		"	for (int i = 0; i < 10; ++i)\n"
		"	{\n"
		"		if (i >= 10)\n"
		"			break;\n"
		"\n"
		"		float offset = float(i) - halfBlur;\n"
		"		texColour = texture2D(albedo, TexCoord + vec2(0,offset / 64)) * Gaussian(offset * strength, deviation);\n"
		"		colour += texColour;\n"
		"	}\n"
	"}\n"
	
	"color= vec4(colour);\n"
	"}\n"
};


static const GLchar* DefaultFrag[] =
{
	"#version 330 core\n"
	"in vec4 ourColor;\n"
	"in float ourTime;\n"
	"in vec2 TexCoord;\n"
	"in vec3 ourNormal;\n"
	"in vec4 gl_FragCoord;\n"
	"out vec4 color;\n"
	"uniform sampler2D albedo;\n"
	"uniform vec4 _my_color;\n"
	"void main()\n"
	"{\n"
	
	//Z-Buffer Line Shader
	"color= _my_color * texture(albedo, TexCoord);\n"
	"}\n"
};

static const GLchar* NonGlowFrag[] =
{
	"#version 330 core\n"
	"in vec4 ourColor;\n"
	"in float ourTime;\n"
	"in vec2 TexCoord;\n"
	"in vec3 ourNormal;\n"
	"in vec4 gl_FragCoord;\n"
	"out vec4 color;\n"
	"uniform sampler2D albedo;\n"
	"uniform vec4 _my_color;\n"
	"void main()\n"
	"{\n"

	//Z-Buffer Line Shader
	"color= vec4(vec3(0),1);\n"
	"}\n"
};


static const GLchar* ShadowMapVert[] =
{
	"#version 330 core\n"
	"layout(location = 0) in vec3 position;\n"
	"layout(location = 1) in vec2 texCoord;\n"
	"layout(location = 2) in vec3 normal;\n"
	"layout(location = 3) in vec4 color;\n"

	"uniform mat4 depthMVP;\n"
	"uniform mat4 model;\n"

	"void main()\n"
	"{\n"
	" gl_Position =  depthMVP *model * vec4(position,1);\n"
	"}\n"
};


static const GLchar* ShadowMapFrag[] =
{
	"#version 330 core\n"
	"layout(location = 0) out vec4 fragmentdepth;\n"

	"void main()\n"
	"{\n"
	"fragmentdepth = vec4(gl_FragCoord.z);\n"
	"}\n"
};


static const  GLchar* UIShaderVert[] =
{
	"#version 330\n"
	"uniform mat4 ProjMtx;\n"
	"uniform mat4 model;\n"
	"uniform vec4 Color_UI_ME;\n"
	"layout(location = 0) in vec3 position;\n"
	"layout(location = 1) in vec2 texCoord;\n"
	"out vec2 Frag_UV;\n"

	"void main()\n"
	"{\n"
	"	Frag_UV = texCoord;\n"
	"	gl_Position = ProjMtx * model * vec4(position.xy,0,1);\n"
	"}\n"
};


static const GLchar* UIShaderFrag[] =
{
	"#version 330\n"
	"uniform sampler2D Texture;\n"
	"uniform vec4 Color_UI_ME;\n"

	"in vec2 Frag_UV;\n"
	"in vec4 Frag_Color;\n"
	"out vec4 Out_Color;\n"
	"void main()\n"
	"{\n"
	"	Out_Color = Color_UI_ME*texture(Texture,vec2(Frag_UV.s, 1.0 - Frag_UV.t).st);\n"
	"}\n"
};

static const GLchar* PointShadowMapVert[] = 
{
	"#version 330 core\n"
	"layout (location = 0) in vec3 position;\n"
	"uniform mat4 model;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = model * vec4(position, 1.0);\n"
	"}\n"
};

static const GLchar* PointShadowMapGeo[] =
{
	"#version 330 core\n"
	"layout (triangles) in;\n"
	"layout (triangle_strip, max_vertices=18) out;\n"
	"uniform mat4 shadow_matrices[6];\n"
	"out vec4 FragPos;\n"
	"void main()\n"
	"{\n"
	"	for(int face = 0; face < 6; ++face)\n"
	"	{\n"
	"		gl_Layer = face;\n"
	"		for(int i = 0; i < 3; ++i)\n"
	"		{\n"
	"			FragPos = gl_in[i].gl_Position;\n"
	"			gl_Position = shadow_matrices[face] * FragPos;\n"
	"			EmitVertex();\n"
	"		}\n"
	"		EndPrimitive();\n"
	"	}\n"
	"}\n"
};

static const GLchar* PointShadowMapFrag[] =
{
	"#version 330 core\n"
	"in vec4 FragPos;\n"
	"uniform vec3 light_pos;\n"
	"uniform float far_plane;\n"
	"void main()\n"
	"{\n"
	"	float light_dist = length(FragPos.xyz - light_pos);\n"
	"	light_dist = light_dist / far_plane; // Normalize it\n"
	"	gl_FragDepth = light_dist;\n"
	"}\n"
};

#endif 