#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec2 jumper_pos;

out vec4 finalColor;

void main() {
    vec4 col = vec4(0.9, 0.9, 0.9, 1.0);

    float dist = distance(jumper_pos, fragTexCoord.xy);
    float limit = 300.0;

    if (dist <= limit) {
        col.a = (limit - dist) / 300.0;
    } else {
        col.a = 0.1;
    }

    finalColor = col;
}
