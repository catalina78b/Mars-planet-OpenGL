#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec4 fPos;
in vec2 fTexCoords;

out vec4 fColor;


//lighting

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform float alpha;
uniform int isLight;

uniform vec3 lightPos1;
uniform vec3 lightPos2;
uniform vec3 lightPos3;
uniform vec3 lightPos4;

uniform vec3 viewPos;

//texture
uniform samplerCube skyBox;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

uniform mat4 view;

vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;
float constant = 1.0f;
float linear = 0.7f;
float quadratic = 1.8f;


vec3 computeLightComponents(float ratio)
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
	//compute ambient light
	ambient += ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse += max(dot(normalEye, lightDirN), 0.0f) * lightColor;

	//compute half vector
	vec3 halfVector = normalize(lightDirN + viewDirN);
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
	specular += specularStrength * specCoeff * lightColor;

      vec3 reflect1=reflect(viewDirN,normalEye);
      //vec3 refract1=refract(viewDirN,normalEye,ratio);

      //vec3 text1=vec3(texture(skyBox,reflect1));

	ambient *= vec3(texture(diffuseTexture, fTexCoords));
	diffuse *= vec3(texture(diffuseTexture, fTexCoords));
	specular *= vec3(texture(specularTexture, fTexCoords));

       //vec4 text2=(texture(skyBox,refract1));
       vec3 color = min((ambient + diffuse) + specular, 1.0f);

       return color;   
  
}
vec3 reflection()
{     vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
     vec3 reflect1=reflect(viewDirN,normalEye);
return reflect1;
}
vec3 refraction(float ratio)
{     vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir);
	
	//compute view direction 
	vec3 viewDirN = normalize(viewPos- fPosEye.xyz);
		
     vec3 refract1=refract(viewDirN,normalEye,ratio);
return refract1;
}
vec3 CalcPointLight(vec3 lightPosEye,vec3 lightColor2)
{

      //transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightPosEye.xyz-fPos.xyz);
	
	//compute view direction 
	vec3 viewDirN = normalize(viewPos- fPos.xyz);

	//compute distance to light
	float dist = length(lightPosEye.xyz - fPos.xyz);//point light
	//compute attenuation
	float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));  
    	//combine results

	//compute ambient light
	ambient += att * ambientStrength * lightColor2;

	//compute diffuse light
	diffuse += att * max(dot(normalEye, lightDirN), 0.0f) * lightColor2;

      //specular
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	specular += specularStrength * specCoeff * lightColor2;

	ambient *= vec3(texture(diffuseTexture, fTexCoords));
	diffuse *= vec3(texture(diffuseTexture, fTexCoords));
	specular *= vec3(texture(specularTexture, fTexCoords));

  	return min(ambient+diffuse+specular,1.0f);
} 
float computeFog()
{
 	float fogDensity = 0.03f;
 	float fragmentDistance = length(fPosEye);
 	float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));
 
 return clamp(fogFactor, 0.0f, 1.0f);
}

vec3 computeSpotLight(vec3 lightPosition, vec3 lightDirection, vec3 lightColor, float cutoff, float outerCutoff) {
	float distance = length(lightPosition - fPos.xyz);
	float att = 1.0f / (constant + linear * distance + quadratic * distance * distance);
	

	//ambient
	ambient += att * ambientStrength * lightColor;

	//diffuse
	vec3 normalEye = normalize(fNormal);
	vec3 lightDirN = normalize(lightPosition - fPos.xyz); //point light

	float theta = dot(lightDirN, normalize(-lightDirection));
	float epsilon = cutoff - outerCutoff;
	float intensity = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);

	diffuse += intensity * att * max(dot(normalEye, lightDirN), 0.0f) * lightColor;

	//spec
	vec3 viewDirN = normalize(viewPos - fPos.xyz);
	vec3 reflection = reflect(-lightDirN, normalEye);
	float spec = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	specular += intensity* att * specularStrength * spec * lightColor;


	ambient *= vec3(texture(diffuseTexture, fTexCoords));
	diffuse *= vec3(texture(diffuseTexture, fTexCoords));
	specular *= vec3(texture(specularTexture, fTexCoords));

  	return min(ambient+diffuse+specular,1.0f);
}
void main() 
{
    ambient = vec3(0.0f, 0.0f, 0.0f);
	diffuse = vec3(0.0f, 0.0f, 0.0f);
	specular = vec3(0.0f, 0.0f, 0.0f);

      float ratioIce=0.763;
      float ratioGlass=0.658;
      vec3 color;
      if (isLight==1)
          color+=computeLightComponents(ratioIce);
      else       
	      color+=CalcPointLight(lightPos3,vec3(0.0f,1.0f,0.0f))+CalcPointLight(lightPos4,vec3(0.0f,1.0f,0.0f))+computeSpotLight(lightPos1,vec3(0.0f, -1.0f,0.0f),vec3(1.0f,1.0f,0.0f),5.5f,10.0f)+computeSpotLight(lightPos2,vec3(0.0f, -1.0f, 0.0f),vec3(1.0f,1.0f,0.0f),5.5f,10.0f);
    
      
      //vec3 refl=reflection();
      //vec3 refr=refraction(ratioIce);

       //vec3 text1=(texture(skyBoxTexture,refl)).rgb;
       //vec3 text2=(texture(skyBoxTexture,refr)).rgb;
       //vec3 color = min((ambient + diffuse) + specular+text1*text2, 1.0f);
    
      fColor = vec4(color, 1.0f);
      float fogFactor = computeFog();
      vec4 fogColor = vec4(0.445f, 0.350f, 0.212f, 1.0f);
      fColor=fogColor * (1-fogFactor)+fColor*fogFactor;
	  fColor.a = alpha;
}