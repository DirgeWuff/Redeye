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

// 8x8 Bayer dither matrix
const float bayerMatrix[64] = float[64](
     0.0, 48.0, 12.0, 60.0,  3.0, 51.0, 15.0, 63.0,
    32.0, 16.0, 44.0, 28.0, 35.0, 19.0, 47.0, 31.0,
     8.0, 56.0,  4.0, 52.0, 11.0, 59.0,  7.0, 55.0,
    40.0, 24.0, 36.0, 20.0, 43.0, 27.0, 39.0, 23.0,
     2.0, 50.0, 14.0, 62.0,  1.0, 49.0, 13.0, 61.0,
    34.0, 18.0, 46.0, 30.0, 33.0, 17.0, 45.0, 29.0,
    10.0, 58.0,  6.0, 54.0,  9.0, 57.0,  5.0, 53.0,
    42.0, 26.0, 38.0, 22.0, 41.0, 25.0, 37.0, 21.0
);

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

    if (brightness > 0.0){
        // Compute Bayer threshold
        int x = int(mod(fragPos.x, 8.0));
        int y = int(mod(fragPos.y, 8.0));
        float threshold = (bayerMatrix[y * 8 + x] + 0.5) / 64.0;

        // Dither
        brightness = step(threshold, brightness) * brightness;
    }

    // Apply light color and brightness to texture
    vec3 light = lightColor / 255.0 * brightness;

    finalColor = vec4(texColor.rgb * light, texColor.a);
}