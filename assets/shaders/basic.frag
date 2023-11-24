#version 430
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;

void main() {
    vec4 texColor = texture(ourTexture, TexCoord);
    // 將 alpha 設置為 1.0 以使背景完全透明
    FragColor = vec4(texColor.rgb, 1.0);
}