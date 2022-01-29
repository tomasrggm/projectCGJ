#version 430

uniform mat4 m_pvm;
uniform mat4 m_viewModel;
uniform mat4 m_model;
uniform mat3 m_normal;
uniform mat4 m_View;
uniform vec4 l_pos;
uniform int fogFlag;
uniform int texMode;
const float density = 0.01;

uniform int dia;
uniform vec4 luzLocal1;
uniform vec4 luzLocal2;
uniform vec4 luzLocal3;
uniform vec4 luzLocal4;
uniform vec4 luzLocal5;
uniform vec4 luzLocal6;
uniform vec4 luzLocal7;
uniform vec4 luzLocal8;
uniform vec4 luzLocal9;
uniform vec4 luzLocal10;
uniform vec4 luzLocal11;
uniform vec4 luzLocal12;
uniform vec4 luzLocal13;
uniform vec4 luzLocal14;
uniform vec4 luzLocal15;
uniform vec4 luzLocal16;

out vec3 lLocalDir1;
out vec3 lLocalDir2;
out vec3 lLocalDir3;
out vec3 lLocalDir4;
out vec3 lLocalDir5;
out vec3 lLocalDir6;
out vec3 lLocalDir7;
out vec3 lLocalDir8;
out vec3 lLocalDir9;
out vec3 lLocalDir10;
out vec3 lLocalDir11;
out vec3 lLocalDir12;
out vec3 lLocalDir13;
out vec3 lLocalDir14;
out vec3 lLocalDir15;
out vec3 lLocalDir16; //lens

in vec4 position;
in vec4 normal;    //por causa do gerador de geometria
in vec4 texCoord;
in vec4 tangent;

out float visibility;

out Data {
	vec3 normal;
	vec3 eye;
	vec3 lightDir;
	vec2 tex_coord;
	vec3 skyboxTexCoord;
	vec3 reflected;
} DataOut;

out vec4 pos;

void main () {

	vec3 aux;
	vec3 n, t, b;

	pos = m_viewModel * position;
	vec4 posLuz = m_model * position;

	DataOut.normal = normalize(m_normal * normal.xyz);
	if(dia == 1){
		DataOut.lightDir = vec3(l_pos - pos);
	}else{
		DataOut.lightDir = vec3(0.0, 0.0, 0.0);
	}
	DataOut.eye = vec3(-pos);

	DataOut.skyboxTexCoord = vec3(m_model * position);	//Transformação de modelação do cubo unitário 
	DataOut.skyboxTexCoord.x = - DataOut.skyboxTexCoord.x; //Texturas mapeadas no interior logo negar a coordenada x

	DataOut.tex_coord = texCoord.st;

	lLocalDir1 = vec3(luzLocal1 - pos);
	lLocalDir2 = vec3(luzLocal2 - pos);
	lLocalDir3 = vec3(luzLocal3 - pos);
	lLocalDir4 = vec3(luzLocal4 - pos);
	lLocalDir5 = vec3(luzLocal5 - pos);
	lLocalDir6 = vec3(luzLocal6 - pos);
	lLocalDir7 = vec3(luzLocal7 - pos);
	lLocalDir8 = vec3(luzLocal8 - pos);
	lLocalDir9 = vec3(luzLocal9 - pos);
	lLocalDir10 = vec3(luzLocal10 - pos);
	lLocalDir11 = vec3(luzLocal11 - pos);
	lLocalDir12 = vec3(luzLocal12 - pos);
	lLocalDir13 = vec3(luzLocal13 - pos);
	lLocalDir14 = vec3(luzLocal14 - pos);
	lLocalDir15 = vec3(luzLocal15 - pos);
	lLocalDir16 = vec3(luzLocal16 - pos);

	if(texMode == 17)  {  //convert eye and light vectors to tangent space

		//Calculate components of TBN basis in eye space
		t = normalize(m_normal * tangent.xyz);  
		b = tangent.w * cross(n,t);

		aux.x = dot(DataOut.lightDir, t);
		aux.y = dot(DataOut.lightDir, b);
		aux.z = dot(DataOut.lightDir, n);
		DataOut.lightDir = normalize(aux);

		aux.x = dot(DataOut.eye, t);
		aux.y = dot(DataOut.eye, b);
		aux.z = dot(DataOut.eye, n);
		DataOut.eye = normalize(aux);
	}


	if(fogFlag == 1){
		float distance = length(pos);
		visibility = exp(-distance*density);
	}

	gl_Position = m_pvm * position;
}