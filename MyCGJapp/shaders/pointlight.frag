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
uniform sampler2D texmap1;
uniform sampler2D texmap2;
uniform sampler2D texmap3;
uniform sampler2D texmap4;
uniform sampler2D texmap5;
uniform sampler2D texmap6;
uniform sampler2D texmap7;
uniform sampler2D texmap8;
uniform sampler2D texmap9;
uniform sampler2D texmap10;
uniform sampler2D texmap11;
uniform sampler2D texmap12;
uniform sampler2D texmap13;
uniform	sampler2D texUnitDiff;
uniform samplerCube cubeMap;



uniform int pointLights;
uniform vec4 corVariavel;
uniform int dia;
uniform int luzBofia;
uniform int spotlightOn; //0 ligadas 1 desligadas
uniform vec4 spotLightDirection;
uniform int mapa;
uniform int assimp;

in vec3 lLocalDir1;
in vec3 lLocalDir2;
in vec3 lLocalDir3;
in vec3 lLocalDir4;
in vec3 lLocalDir5;
in vec3 lLocalDir6;
in vec3 lLocalDir7;
in vec3 lLocalDir8;
in vec3 lLocalDir9;
in vec3 lLocalDir10;
in vec3 lLocalDir11;
in vec3 lLocalDir12;
in vec3 lLocalDir13;
in vec3 lLocalDir14; //spotlight
in vec3 lLocalDir15; //spotlight
in vec3 lLocalDir16; //lens

uniform int fogFlag;
in vec4 pos;
in float visibility;

in Data {
	vec3 normal;
	vec3 eye;
	vec3 lightDir;
	vec2 tex_coord;
	vec3 skyboxTexCoord;
} DataIn;

void main() {
	vec4 texel, texel1;
	vec4 branco = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 vermelho = vec4(1.0, 0.0, 0.0, 1.0);
	vec4 azul = vec4(0.0, 0.3, 1.0, 1.0);
	float constant = 1.0f;
	float linear = 0.014f;
	float quadratic = 0.0007f;

	vec3 h;
	vec3 n = normalize(DataIn.normal);
	vec3 l = normalize(DataIn.lightDir);
	vec3 e = normalize(DataIn.eye);

	vec4 spec = vec4(0.0);
	float intensity = max(dot(n,l), 0.0);
	float intSpec;
	vec4 resultado = vec4(0.0);
	float dist, attenuation;
	float l_spotCutOff = 0.997f;
	vec4 l_spotDir = spotLightDirection;
	
	if (intensity > 0.0) {

		vec3 h = normalize(l + e);
		float intSpec = max(dot(h,n), 0.0);
		spec = mat.specular * pow(intSpec, mat.shininess);
		resultado = branco * intensity;
	}
	
	if(pointLights == 1){
			dist = length(lLocalDir1);
			l = normalize(lLocalDir1);
			intensity = max(dot(n,l), 0.0);
			h = normalize(l + e);
			intSpec = max(dot(h,n), 0.0);
			spec += mat.specular * pow(intSpec, mat.shininess);
			attenuation = 1.0 / (constant + linear * dist + quadratic * (dist * dist));
			resultado += corVariavel * intensity * attenuation ;

			dist = length(lLocalDir2);
			l = normalize(lLocalDir2);
			intensity = max(dot(n,l), 0.0);
			h = normalize(l + e);
			intSpec = max(dot(h,n), 0.0);
			spec += (mat.specular * pow(intSpec, mat.shininess)) * attenuation;
			attenuation = 1.0 / (constant + linear * dist + quadratic * (dist * dist));
			resultado += corVariavel * intensity * attenuation;
	
			dist = length(lLocalDir3);
			l = normalize(lLocalDir3);
			intensity = max(dot(n,l), 0.0);
			h = normalize(l + e);
			intSpec = max(dot(h,n), 0.0);
			spec += (mat.specular * pow(intSpec, mat.shininess)) * attenuation;
			attenuation = 1.0 / (constant + linear * dist + quadratic * (dist * dist));
			resultado += corVariavel * intensity * attenuation;
		
			dist = length(lLocalDir4);
			l = normalize(lLocalDir4);
			intensity = max(dot(n,l), 0.0);
			h = normalize(l + e);
			intSpec = max(dot(h,n), 0.0);
			spec += (mat.specular * pow(intSpec, mat.shininess)) * attenuation;
			attenuation = 1.0 / (constant + linear * dist + quadratic * (dist * dist));
			resultado += corVariavel * intensity * attenuation ;
		
			dist = length(lLocalDir5);
			l = normalize(lLocalDir5);
			intensity = max(dot(n,l), 0.0);
			h = normalize(l + e);
			intSpec = max(dot(h,n), 0.0);
			spec += (mat.specular * pow(intSpec, mat.shininess)) * attenuation;
			attenuation = 1.0 / (constant + linear * dist + quadratic * (dist * dist));
			resultado += corVariavel * intensity * attenuation;
			
			dist = length(lLocalDir6);
			l = normalize(lLocalDir6);
			intensity = max(dot(n,l), 0.0);
			h = normalize(l + e);
			intSpec = max(dot(h,n), 0.0);
			spec += (mat.specular * pow(intSpec, mat.shininess)) * attenuation;
			attenuation = 1.0 / (constant + linear * dist + quadratic * (dist * dist));
			resultado +=  corVariavel * intensity * attenuation;

			dist = length(lLocalDir7);
			l = normalize(lLocalDir7);
			intensity = max(dot(n,l), 0.0);
			h = normalize(l + e);
			intSpec = max(dot(h,n), 0.0);
			spec += (mat.specular * pow(intSpec, mat.shininess)) * attenuation;
			attenuation = 1.0 / (constant + linear * dist + quadratic * (dist * dist));
			resultado += corVariavel * intensity * attenuation;

			dist = length(lLocalDir8);
			l = normalize(lLocalDir8);
			intensity = max(dot(n,l), 0.0);
			h = normalize(l + e);
			intSpec = max(dot(h,n), 0.0);
			spec += (mat.specular * pow(intSpec, mat.shininess)) * attenuation;
			attenuation = 1.0 / (constant + linear * dist + quadratic * (dist * dist));
			resultado += corVariavel * intensity * attenuation;

			dist = length(lLocalDir9);
			l = normalize(lLocalDir9);
			intensity = max(dot(n,l), 0.0);
			h = normalize(l + e);
			intSpec = max(dot(h,n), 0.0);
			spec += (mat.specular * pow(intSpec, mat.shininess)) * attenuation;
			attenuation = 1.0 / (constant + linear * dist + quadratic * (dist * dist));
			resultado += branco * intensity * attenuation;

			dist = length(lLocalDir10);
			l = normalize(lLocalDir10);
			intensity = max(dot(n,l), 0.0);
			h = normalize(l + e);
			intSpec = max(dot(h,n), 0.0);
			spec += (mat.specular * pow(intSpec, mat.shininess)) * attenuation;
			attenuation = 1.0 / (constant + linear * dist + quadratic * (dist * dist));
			resultado += branco * intensity * attenuation;

			dist = length(lLocalDir11);
			l = normalize(lLocalDir11);
			intensity = max(dot(n,l), 0.0);
			h = normalize(l + e);
			intSpec = max(dot(h,n), 0.0);
			spec += (mat.specular * pow(intSpec, mat.shininess)) * attenuation;
			attenuation = 1.0 / (constant + linear * dist + quadratic * (dist * dist));
			resultado += branco * intensity * attenuation;

			dist = length(lLocalDir12);
			l = normalize(lLocalDir12);
			intensity = max(dot(n,l), 0.0);
			h = normalize(l + e);
			intSpec = max(dot(h,n), 0.0);
			spec += (mat.specular * pow(intSpec, mat.shininess)) * attenuation;
			attenuation = 1.0 / (constant + linear * dist + quadratic * (dist * dist));
			resultado += branco * intensity * attenuation;

			//lens flare
			dist = length(lLocalDir16);
			l = normalize(lLocalDir12);
			intensity = max(dot(n,l), 0.0);
			h = normalize(l + e);
			intSpec = max(dot(h,n), 0.0);
			spec += (mat.specular * pow(intSpec, mat.shininess)) * attenuation;
			attenuation = 1.0 / (constant + linear * dist + quadratic * (dist * dist));
			resultado += branco * intensity * attenuation;

			if(dia == 0){
				dist = length(lLocalDir13);
				l = normalize(lLocalDir13);
				intensity = max(dot(n,l), 0.0);
				h = normalize(l + e);
				intSpec = max(dot(h,n), 0.0);
				spec += (mat.specular * pow(intSpec, mat.shininess)) * attenuation;
				attenuation = 1.0 / (1.0 + 0.0014 * dist + 0.00001 * (dist * dist));
				if(luzBofia == 0){
					//resultado += vec4(1.0, 0.75, 0.0, 1.0) * intensity * attenuation ;
					resultado += vec4(1.0, 0.0, 0.0, 1.0) * intensity * attenuation ;
				}else{
					resultado += vec4(0.0, 0.0, 1.0, 1.0) * intensity * attenuation ;
				}
				
			}
	}
	if(spotlightOn == 0 && mapa == 0){
			float intensity = 0.0;
			vec4 spec = vec4(0.0);
			vec3 ld = normalize(lLocalDir14);
			vec3 sd = normalize(vec3(-l_spotDir));
			if (dot(sd,ld) > l_spotCutOff) {
				vec3 n = normalize(DataIn.normal);
				intensity = max(dot(n,ld), 0.0);
				if (intensity > 0.0) {
					vec3 eye = normalize(DataIn.eye);
					vec3 h = normalize(ld + eye);
					float intSpec = max(dot(h,n), 0.0);
					spec = mat.specular * pow(intSpec, mat.shininess);
					resultado += (spec + intensity*mat.diffuse) ;
				}
			}
			intensity = 0.0;
			spec = vec4(0.0);
			ld = normalize(lLocalDir15);
			sd = normalize(vec3(-l_spotDir));
			if (dot(sd,ld) > l_spotCutOff) {
				vec3 n = normalize(DataIn.normal);
				intensity = max(dot(n,ld), 0.0);
				if (intensity > 0.0) {
					vec3 eye = normalize(DataIn.eye);
					vec3 h = normalize(ld + eye);
					float intSpec = max(dot(h,n), 0.0);
					spec = mat.specular * pow(intSpec, mat.shininess);
					resultado += (spec + intensity*mat.diffuse) ;
				}
			}
	}

	//texturas
	if (texMode == 1){
			texel = texture(texmap, DataIn.tex_coord);  // texel from helice
			if(texel.a == 0.0) discard;
			else
				//colorOut =  vec4(max(resultado*texel.rgb + spec.rgb, texel.rgb), texel.a);
				colorOut =  vec4(max(resultado.rgb*texel.rgb + spec.rgb, texel.rgb), texel.a);
	}else if(texMode == 2){//grass
			texel = texture(texmap1, DataIn.tex_coord);  // texel from grass
			colorOut =  vec4(max(resultado.rgb*texel.rgb + spec.rgb,  mat.ambient.rgb  + 0.1*texel.rgb), 1.0);

	}else if(texMode == 3){//sign
			texel = texture(texmap3, DataIn.tex_coord);  // texel from cola
			texel1 = texture(texmap2, DataIn.tex_coord);  // texel from lines
			colorOut =  vec4(max(resultado.rgb*texel.rgb*texel1.rgb + spec.rgb,  mat.ambient.rgb  + 0.1*texel.rgb*texel.rgb), 1.0);
	}else if(texMode == 4){//sign
			texel = texture(texmap4, DataIn.tex_coord);  // texel from banana
			texel1 = texture(texmap2, DataIn.tex_coord);  // texel from lines
			colorOut =  vec4(max(resultado.rgb*texel.rgb*texel1.rgb + spec.rgb,  mat.ambient.rgb  + 0.1*texel.rgb*texel.rgb), 1.0);
	}else if(texMode == 5){//sign
			texel = texture(texmap5, DataIn.tex_coord);  // texel from genshin
			texel1 = texture(texmap2, DataIn.tex_coord);  // texel from lines
			colorOut =  vec4(max(resultado.rgb*texel.rgb*texel1.rgb + spec.rgb,  mat.ambient.rgb  + 0.1*texel.rgb*texel.rgb), 1.0);
	}else if(texMode == 6){//sign
			texel = texture(texmap6, DataIn.tex_coord);  // texel from jumbo
			texel1 = texture(texmap2, DataIn.tex_coord);  // texel from lines
			colorOut =  vec4(max(resultado.rgb*texel.rgb*texel1.rgb + spec.rgb,  mat.ambient.rgb  + 0.1*texel.rgb*texel.rgb), 1.0);	
	}else if(texMode == 7){//sign
			texel = texture(texmap7, DataIn.tex_coord);  // texel from lidl
			texel1 = texture(texmap2, DataIn.tex_coord);  // texel from lines
			colorOut =  vec4(max(resultado.rgb*texel.rgb*texel1.rgb + spec.rgb,  mat.ambient.rgb  + 0.1*texel.rgb*texel.rgb), 1.0);	
	}else if(texMode == 8){//sign
			texel = texture(texmap8, DataIn.tex_coord);  // texel from lidl
			texel1 = texture(texmap2, DataIn.tex_coord);  // texel from lines
			colorOut =  vec4(max(resultado.rgb*texel.rgb*texel1.rgb + spec.rgb,  mat.ambient.rgb  + 0.1*texel.rgb*texel.rgb), 1.0);	
	}else if(texMode == 9){//sign
			texel = texture(texmap9, DataIn.tex_coord);  // texel from lidl
			texel1 = texture(texmap2, DataIn.tex_coord);  // texel from lines
			colorOut =  vec4(max(resultado.rgb*texel.rgb*texel1.rgb + spec.rgb,  mat.ambient.rgb  + 0.1*texel.rgb*texel.rgb), 1.0);		
	}else if(texMode == 10){//balas
			colorOut =  vec4(max(resultado*mat.diffuse + spec, mat.ambient).rgb, mat.diffuse.a);
			if(dia == 1){
				colorOut = colorOut * vec4(1.3,1.3,1.3,1.0);
			}else{
				colorOut = vec4(1.0,0.02,0.95,1.0);
			}
	}else if(texMode == 11){//balas
			colorOut =  vec4(max(resultado*mat.diffuse + spec, mat.ambient).rgb, mat.diffuse.a);
			if(dia == 1){
				colorOut = colorOut * vec4(1.3,1.3,1.3,1.0);
			}else{
				colorOut = vec4(1.0,0.9,0.95,1.0);
			}
	}else if(texMode == 12){//outros aviones
			colorOut =  vec4(max(resultado*mat.diffuse + spec, mat.ambient).rgb, mat.diffuse.a);
			if(dia == 1){
				colorOut = colorOut * vec4(1.3,1.3,1.3,1.0);
			}else{
				colorOut = vec4(0.51,0.93,1.0,1.0);
			}
	}else if (texMode == 13 && assimp == 0 && dia == 1){
			texel = texture(texmap10, DataIn.tex_coord);  // texel from plane
			if(texel.a == 0.0) discard;
			else
			colorOut =  vec4(texel.rgb, 1.0);
	}else if (texMode == 13 && assimp == 0 && dia == 0){
			texel = texture(texmap11, DataIn.tex_coord);  // texel from plane
			if(texel.a == 0.0) discard;
			else
			colorOut =  vec4(texel.rgb, texel.a);
	}else if (texMode == 14){
			texel = texture(texmap, DataIn.tex_coord); //texel from particla (que na verdade e a flor hihi)
			if((texel.a == 0.0)  || (mat.diffuse.a == 0.0) ) 
				discard;
			else
				colorOut = mat.diffuse;
	}else if(texMode == 15){
		texel = texture(texmap13, DataIn.tex_coord);  //texel from element flare texture
		if((texel.a == 0.0)  || (mat.diffuse.a == 0.0) ) discard;
		else
			colorOut = mat.diffuse * texel;
	}else if(texMode == 16){
		colorOut = texture(cubeMap, DataIn.skyboxTexCoord);
	}else{
		//colorOut = vec4(max(resultado * mat.diffuse + spec, mat.ambient).rgb, mat.diffuse.a);
		colorOut =  vec4(max(resultado*mat.diffuse + spec, mat.ambient).rgb, mat.diffuse.a);
	}

	//Luz neon para os billboards
	if(texMode > 2 && texMode < 10 && dia==0){
        colorOut = vec4(max(resultado.rgb * texel.rgb * texel1.rgb + spec.rgb, texel.rgb*texel1.rgb), 1.0);
        colorOut = colorOut * vec4(2.0,2.0,2.0,1.0);
    }else if(texMode > 2 && texMode < 10 && dia==1){
        colorOut = vec4(max(resultado.rgb * texel.rgb * texel1.rgb + spec.rgb, texel.rgb*texel1.rgb), 1.0);
        colorOut = colorOut * vec4(1.3,1.3,1.3,1.0);
    }

	//Comboio assimp
	if(assimp == 1 && dia == 1){
		texel = texture(texUnitDiff, DataIn.tex_coord);
		colorOut = vec4(max(resultado.rgb*texel.rgb + spec.rgb,  mat.ambient.rgb  + 0.1*texel.rgb), 1.0);	
	}
	else if(assimp == 1 && dia == 0){ //Luz neon para o comboio
		texel = texture(texUnitDiff, DataIn.tex_coord);
		colorOut = vec4(max(resultado.rgb*texel.rgb + spec.rgb,  mat.ambient.rgb  + 0.1*texel.rgb), 1.0);
		colorOut = colorOut * vec4(8.0,8.0,8.0,1.0);
	}

	if(fogFlag == 1){
		vec3 fogColor = vec3(0.5,0.6,0.7);
		colorOut = mix(vec4(fogColor,1.0), colorOut, visibility);
	}
	
}
