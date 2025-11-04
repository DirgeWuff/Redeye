#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D texture0;

// Constants
uniform vec3 lightColor = vec3(122.0, 122.0, 122.0);
uniform float lightRadius = 800.0;
uniform float intensity = 1.0;
uniform float beamAngle = 1.0472;
uniform float beamSoftness = 0.2;

// Non-constants
uniform vec2 screenResolution;
uniform vec2 lightPos;
uniform vec2 lightDirection;

void main() {
    // Sample the base texture
    vec4 texColor = texture(texture0, fragTexCoord);
    vec2 fragPos = fragTexCoord * screenResolution;

    // Vector from light to fragment
    vec2 toFrag = fragPos - lightPos;
    float distance = length(toFrag);
    vec2 dirToFrag = normalize(toFrag);

    // Distance-based attenuation. Quadratic for smooth falloff
    float attenuation = pow(clamp(1.0 - distance / lightRadius, 0.0, 1.0), 2.0);

    // Beam attenuation
    float halfAngle = beamAngle * 0.5;
    float outerBeam = cos(halfAngle);
    float innerBeam = cos(halfAngle * (1.0 - beamSoftness));
    float angleCos = dot(normalize(lightDirection), dirToFrag);
    float beamAttenuation = smoothstep(outerBeam, innerBeam, angleCos);

    // Combine distance and cone attenuation
    float brightness = attenuation * beamAttenuation * intensity;

    // Apply light color and brightness to texture
    vec3 light = lightColor / 255.0 * brightness;

    finalColor = vec4(texColor.rgb * light, texColor.a);
}