#version 330

in vec3 fragPosition;

uniform samplerCube environmentMap;

out vec4 finalColor;

void main() {
    vec3 color = texture(environmentMap, fragPosition).rgb;
    // Subtle tone-mapping
    color = color / (color + vec3(1.0));
    finalColor = vec4(color, 1.0);
}
