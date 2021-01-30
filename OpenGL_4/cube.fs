#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixValue;

uniform float brightness;
uniform float contrast;
uniform float saturation;

vec3 colorAdjust(vec3 srcColor)
{
	// 亮度
	vec3 destColor = srcColor * brightness;

    //饱和度
    float Luminance = 0.215 * srcColor.r + 0.7154 * srcColor.g + 0.0721 *srcColor.b;
    vec3 LuminanceColor = vec3(Luminance,Luminance,Luminance);
    destColor = mix(LuminanceColor, destColor, saturation);

    //对比度
    vec3 avgColor = vec3(0.5,0.5,0.5);
    destColor = mix(avgColor, destColor, contrast);

	return destColor;
}

void main()
{
	vec4 tmp = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), mixValue);
	FragColor = vec4(colorAdjust(tmp.rgb), 1.0);
}