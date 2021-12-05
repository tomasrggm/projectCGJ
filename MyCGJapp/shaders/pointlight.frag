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
uniform int luzAmbiente;
uniform int luzDifusa;
uniform int luzHolofote;
float l_spotCutOff = 0.99f;
vec4 l_spotDir = vec4(0.0,0.0,-1.0,1.0);


in Data {
	vec3 normal;
	vec3 eye;
	vec3 lightDir;
} DataIn[9];

void main() {

	vec4 spec = vec4(0.0);
	vec4 accum = vec4(0.0);
	for(int i = 0; i < 9; i++){
		if(i < 6){
			vec3 n = normalize(DataIn[i].normal);
			vec3 l = normalize(DataIn[i].lightDir);
			vec3 e = normalize(DataIn[i].eye);

			float intensity = max(dot(n,l), 0.0);

	
			if (intensity > 0.0) {

				vec3 h = normalize(l + e);
				float intSpec = max(dot(h,n), 0.0);
				spec = mat.specular * pow(intSpec, mat.shininess);
				accum += (intensity * mat.diffuse + spec) * luzDifusa;
			}
		}else if(i > 6){ //ignorar 6 que e a luz global
			float intensity = 0.0;
			vec4 spec = vec4(0.0);
			vec3 ld = normalize(DataIn[i].lightDir);
			vec3 sd = normalize(vec3(-l_spotDir));
			if (dot(sd,ld) > l_spotCutOff) {
				vec3 n = normalize(DataIn[i].normal);
				intensity = max(dot(n,ld), 0.0);
				if (intensity > 0.0) {
					vec3 eye = normalize(DataIn[i].eye);
					vec3 h = normalize(ld + eye);
					float intSpec = max(dot(h,n), 0.0);
					spec = mat.specular * pow(intSpec, mat.shininess);
					accum += (spec + intensity*mat.diffuse) * luzHolofote;
				}
			}
		}

	}

	vec3 n = normalize(DataIn[6].normal);
	vec3 l = normalize(DataIn[6].lightDir);
	vec3 e = normalize(DataIn[6].eye);

	float intensity = max(dot(n,l), 0.0);
	if (intensity > 0.0) {
		vec3 h = normalize(l + e);
		float intSpec = max(dot(h,n), 0.0);
		spec = mat.specular * pow(intSpec, mat.shininess);
		}
	colorOut = max(accum, (spec + intensity*mat.diffuse) * luzAmbiente);
}