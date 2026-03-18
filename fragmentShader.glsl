#version 330 core
out vec4 FragColor;

uniform vec2 u_pos;      // A kör középpontja (pixelben: 0-600)
uniform bool u_inverted; // Színfelcserélés állapota

void main() {
    // gl_FragCoord.xy az aktuális pixel koordinátája (0,0-tól 600,600-ig)
    vec2 st = gl_FragCoord.xy;
    float d = distance(st, u_pos);
    
    // Alapszín: Sárga (háttér)
    vec3 color = vec3(173.0f/255, 153.0f/255, 17.0f/255);

    // Kék vízszintes szakasz rajzolása (x: 200-400, y: 300 környéke)
    if (st.y >= 298.5 && st.y <= 301.5 && st.x >= 200.0 && st.x <= 400.0) {
        color = vec3(0.0, 0.0, 1.0);
    }

    // Kör rajzolása (r = 50)
    if (d <= 50.0) {
        float t = d / 50.0;
        if (u_inverted) {
            // Felcserélt: Zöld közép -> Piros szél
            color = mix(vec3(0.0, 1.0, 0.0), vec3(1.0, 0.0, 0.0), t);
        } else {
            // Eredeti: Piros közép -> Zöld szél
            color = mix(vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), t);
        }
    }

    FragColor = vec4(color, 1.0);
}