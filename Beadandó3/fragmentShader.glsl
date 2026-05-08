#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform bool lightOn;
uniform bool isLightSource;

void main() {
    
    vec3 ambientLight = vec3(1.0f,1.0f,1.0f)    //ambient fény
    vec3 lightColor = vec3(1.0f, 0.8f, 0.2f);   //keringő fény szín
    vec3 objectColor = vec3(1.0f, 1.0f, 1.0f);  //kocka szín

    if(isLightSource) {
        FragColor = vec4(lightColor, 1.0);
        return;
    }

    vec3 finalAmbient;
    vec3 diffuse = vec3(0.0);

    if(lightOn)
    {  	
       finalAmbient = 0.2 * lightColor;
       
       vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diffIntenzitas = max(dot(norm, lightDir), 0.0);
        diffuse = diffIntenzitas * lightColor;
    } 
    else {
        
        finalAmbient = ambientLight; 
    }

    // Végeredmény kiszámítása
    vec3 result = (finalAmbient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
    
}