#version 430
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;

void main() {
    vec4 texColor = texture(ourTexture, TexCoord);
    // �N alpha �]�m�� 1.0 �H�ϭI�������z��
    FragColor = vec4(texColor.rgb, 1.0);
}