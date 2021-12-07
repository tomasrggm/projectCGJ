#version 430

out vec4 colorOut;

struct Materials {
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	vec4 emissive;
	float shininess;
	int texCount;
};

uniform Materials mat;
uniform int texMode;
uniform sampler2D texmap;
uniform int fogFlag;
in vec4 pos;
const float density = 0.01;

in Data {
	vec3 normal;
	vec3 eye;
	vec3 lightDir;
	vec2 tex_coord;
} DataIn;

void main() {
	vec4 texel;

	vec4 spec = vec4(0.0);

	vec3 n = normalize(DataIn.normal);
	vec3 l = normalize(DataIn.lightDir);
	vec3 e = normalize(DataIn.eye);

	float intensity = max(dot(n,l), 0.0);

	
	if (intensity > 0.0) {

		vec3 h = normalize(l + e);
		float intSpec = max(dot(h,n), 0.0);
		spec = mat.specular * pow(intSpec, mat.shininess);
	}
	
	//texturas
	if (texMode == 1){
			texel = texture(texmap, DataIn.tex_coord);  // texel from helice
			if(texel.a == 0.0) discard;
			else
				colorOut = vec4(max(intensity*texel.rgb + spec.rgb, texel.rgb), texel.a);

	}else{
		colorOut = vec4(max(intensity * mat.diffuse + spec, mat.ambient).rgb, mat.diffuse.a);
	}

	//seccao do fog
	vec3 fogColor = vec3(0.5,0.6,0.7);
	float distance = length(pos);
	float visibility = exp(-distance*density);
	colorOut[3] = mat.diffuse.a; 
	if(fogFlag == 0){
		colorOut = mix(vec4(fogColor,1.0), colorOut, visibility);
	}
	
}