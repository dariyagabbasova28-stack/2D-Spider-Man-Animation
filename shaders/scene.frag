#version 330 core
in vec3 vColor;
in vec2 vWorld;
out vec4 FragColor;
uniform vec2 uLightPos;
uniform bool uLighting;
uniform bool uOverlay;
void main() {
    vec3 color=vColor;
    if(uLighting && !uOverlay) {
        // Point-light diffuse on flat, front-facing 2D surfaces (2.5D lighting).
        vec3 N=vec3(0.0,0.0,1.0);
        vec3 L=normalize(vec3(uLightPos-vWorld,0.35));
        float diffuse=max(dot(N,L),0.0);
        float distanceToLight=length(uLightPos-vWorld);
        float attenuation=1.0/(1.0+0.55*distanceToLight*distanceToLight);
        float brightness=0.30+1.05*diffuse*attenuation;
        color=clamp(color*brightness,0.0,1.0);
    }
    FragColor=vec4(color,1.0);
}
