#version 150 core

 uniform sampler2D diffuseTex ;
uniform sampler2D diffuseTex2 ;
 uniform samplerCube cubeTex ;

 uniform float changetex;
 uniform vec4 lightColour ;
 uniform vec3 lightPos ;
 uniform vec3 cameraPos ;
 uniform float lightRadius ;

 in Vertex {
 vec4 colour ;
 vec2 texCoord ;
 vec3 normal ;
 vec3 worldPos ;
 } IN ;

 out vec4 fragColour ;
void main (void) {
	vec4 diffuse = texture (diffuseTex, IN.texCoord);
	vec4 diffuse2 = texture (diffuseTex2, IN.texCoord);
	vec4 mixture = mix(diffuse, diffuse2, changetex) * IN.colour;


 vec3 incident = normalize ( IN.worldPos - cameraPos );
 float dist = length ( lightPos - IN.worldPos );
 float atten = 1.0 - clamp ( dist / lightRadius , 0.2 , 1.0);
 vec4 reflection = texture ( cubeTex ,reflect ( incident , normalize ( IN.normal )));

 fragColour = ( lightColour * mixture * atten )*( mixture + reflection );
 }