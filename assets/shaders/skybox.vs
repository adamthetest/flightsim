#version 330

in vec3 vertexPosition;

uniform mat4 matProjection;
uniform mat4 matView;

out vec3 fragPosition;

void main() {
    fragPosition = vertexPosition;
    // Remove translation from the view matrix so skybox stays centered on camera
    mat4 rotView = mat4(mat3(matView));
    vec4 clipPos = matProjection * rotView * vec4(vertexPosition, 1.0);
    // Set z to w so depth is always 1.0 (behind everything)
    gl_Position = clipPos.xyww;
}
