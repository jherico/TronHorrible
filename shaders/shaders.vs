#define LIGHTING_COMMON
uniform   vec3 Ambient;
uniform   vec4 LightPos[8];
uniform   vec4 LightColor[8];
uniform   float LightCount;
varying   vec4 oColor;
varying   vec2 oTexCoord;
varying   vec3 oNormal;
varying   vec3 oVPos;

vec4 DoLight()
{
   vec3 norm = normalize(oNormal);
   vec3 light = Ambient;
   for (int i = 0; i < int(LightCount); i++)
   {
       vec3 ltp = (LightPos[i].xyz - oVPos);
       float  ldist = length(ltp);
       ltp = normalize(ltp);
       light += clamp(LightColor[i].rgb * oColor.rgb * (dot(norm, ltp) / ldist), 0.0,1.0);
   }
   return vec4(light, oColor.a);
}


static const char* LitSolidFragShaderSrc =
    LIGHTING_COMMON
	void main()
	{
	   gl_FragColor = DoLight() * oColor;
	}

static const char* LitTextureFragShaderSrc =
	uniform sampler2D Texture0;
    LIGHTING_COMMON
	void main()
	{
	   gl_FragColor = DoLight() * texture2D(Texture0, oTexCoord);
	}

static const char* AlphaTextureFragShaderSrc =
	uniform sampler2D Texture0;
	varying vec4 oColor;
	varying vec2 oTexCoord;
	void main()
	{
	   gl_FragColor = oColor * vec4(1,1,1,texture2D(Texture0, oTexCoord).a);
	}

static const char* MultiTextureFragShaderSrc =
	uniform sampler2D Texture0;
	uniform sampler2D Texture1;
	varying vec4 oColor;
	varying vec2 oTexCoord;
	varying vec2 oTexCoord1;
	void main()
	{
	   vec4 color1 = texture2D(Texture0, oTexCoord);
	   vec4 color2 = texture2D(Texture1, oTexCoord1);
	   color2.rgb = color2.rgb * mix(1.9, 1.2, clamp(length(color2.rgb),0.0,1.0));
	   color2 = color1 * color2;
	   if (color2.a <= 0.6)
	       discard;
	   gl_FragColor = color2;
	}
