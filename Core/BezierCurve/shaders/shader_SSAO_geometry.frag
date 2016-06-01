#version 330 core
layout (location = 0) out vec4 gPositionDepth;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

const float NEAR = 0.1; // Projection matrix's near plane distance
const float FAR = 50.0f; // Projection matrix's far plane distance
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}

void main()
{
    // Store the fragment position vector in the first gbuffer texture
    gPositionDepth.xyz = FragPos;
    
//    gPositionDepth.xyz = vec3(-1.774541, 4.895270f, 2.554885f);
    // And store linear depth into gPositionDepth's alpha component
//    int one = 1;
//    gPositionDepth.a = (LinearizeDepth(gl_FragCoord.z)); // Divide by FAR if you need to store depth in range 0.0 - 1.0 (if not using floating point colorbuffer)
    
    gPositionDepth.a = (LinearizeDepth(gl_FragCoord.z));
//    gPositionDepth.a = (gPositionDepth.a > 50.0f) ? 50.0f : gPositionDepth.a;
    
    
    // Also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);
    // And the diffuse per-fragment color
    gAlbedoSpec.rgb = vec3(0.95); // Currently all objects have constant albedo color
}