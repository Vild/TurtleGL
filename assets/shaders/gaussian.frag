#version 430 core

layout (location = 0) out vec4 gaussianImage;
in vec2 vUV;
uniform sampler2D diffuseTexture;

uniform float offset[5] = float[]( 0.0, 1.0, 2.0, 3.0, 4.0 );
uniform float weight[5] = float[]( 0.2270270270, 0.1945945946, 0.1216216216,
                                   0.0540540541, 0.0162162162 );
uniform float screenWidth;
uniform float screenHeight;
uniform bool horizontal;
uniform bool setting_enableGaussian;

void main() {
	if(setting_enableGaussian){
		vec3 result = texture(diffuseTexture, vUV).xyz * weight[0]; // current fragment's contribution
		vec2 tex_offset = 1.0 / textureSize(diffuseTexture, 0); // gets size of single texel
		if(horizontal){
			for(int i = 1; i < 5; i++){
				result += texture(diffuseTexture, vUV + vec2(tex_offset.x * i, 0.0)).xyz * weight[i];
			    result += texture(diffuseTexture, vUV - vec2(tex_offset.x * i, 0.0)).xyz * weight[i];
			}
		}else{
			for(int i = 1; i < 5; i++)
			{
			    result += texture(diffuseTexture, vUV + vec2(0.0, tex_offset.y * i)).xyz * weight[i];
			    result += texture(diffuseTexture, vUV - vec2(0.0, tex_offset.y * i)).xyz * weight[i];
			}
		}
		gaussianImage = vec4(result, 1.0);
	}else
		gaussianImage = vec4(texture(diffuseTexture, vUV).xyz, 1.0f);
}