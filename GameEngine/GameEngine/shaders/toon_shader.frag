#version 330 core


//out vec4 color;
//
//void main()
//{
//    color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
//}
//
//
//
//
//


uniform mat4 model;
uniform vec3 eye; // Camera position is the 'eye'

// Material settings
uniform struct Material
{
    vec3 k_a; // Ambient coefficient, it is used to define the "inherent color" of the object.
    vec3 k_d; // Material Diffuse reflectance, diffuse material is a material that reflects light rays in many directions
    vec3 k_s; // Specular coefficient, A Specular material is to simulate the effect of shiny objects(such as in metals) creating a "spec" where the reflection direction and eye match up.
    float shininess; // Denoted alpha. It is how much the specular is dispersed over the object. The higher this value, the smaller the highlight "spec".
} material;

// Light settings
uniform struct DirectionalLight {
    bool on;
    vec3 direction;
    vec3 c_i; // c_i - light color intensity, a.k.a the color of the light
    float L_a; // Ambient coefficient, it is how much ambient influence the light emits. If this value is 1, the object's "inherent color" will shine through perfectly.
    float L_d; // Diffuse coefficient, it is how much of the color the light emits will be diffused.
    float L_s; //Specular coefficient, it is how much specular influence the light emit.
} directionalLight;


in vec3 fragNormal;
in vec3 fragVert;

out vec4 finalColor;

void main()
{
    vec3 surfacePos = vec3(model * vec4(fragVert, 1.0f)); // p - surface position in world space
    
    if (directionalLight.on)
    {
        vec3 n = normalize(transpose(inverse(mat3(model))) * fragNormal);  // n - unit surface normal
        vec3 l = normalize(-directionalLight.direction); // l - unit light direction, l = -d
        vec3 r = reflect(-l, n); // r - unit reflection vector based on l and n. This is the reflected light normalized
        vec3 v = normalize(eye - surfacePos); // v - unit eye direction, or the surface normal to eye direction.
        
        vec3 specular = vec3(0);
        
        if (dot(l, n) > 0.0)
        {
            //specular
            specular = directionalLight.c_i * material.k_s * directionalLight.L_s * pow(max(0.0f, dot(v, r)), material.shininess);
        }
        
        //ambient
        vec3 ambient = directionalLight.c_i * directionalLight.L_a * material.k_a;
        
        //diffuse
        vec3 diffuse = directionalLight.c_i * material.k_d * directionalLight.L_d * max(0.0f, dot(n, l));
        
        // Use 0.6 as edge detection coefficient
        float edgeDetection = (dot(v, n) > 0.6) ? 1 : 0;
        
        //linear color (color before gamma correction)
        vec3 linearColor = edgeDetection * (ambient + diffuse + specular);
        
        finalColor = vec4(linearColor, 1.0f);
    }
    else //there is no light
    {
        finalColor = vec4(0, 0, 0, 1);
    }
    
    
}