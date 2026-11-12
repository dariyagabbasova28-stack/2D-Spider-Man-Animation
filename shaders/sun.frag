#version 330 core
in vec3 vNormal;
in vec3 vWorld;
out vec4 FragColor;
uniform vec2 uLightPos;
uniform bool uLighting;
void main() {
    vec3 N=normalize(vNormal);
    vec3 L=normalize(vec3(-0.6,0.9,1.5));
    vec3 V=normalize(vec3(0.0,0.0,3.15)-vWorld);
    float diffuse=max(dot(N,L),0.0);
    float spec=pow(max(dot(N,normalize(L+V)),0.0),18.0);
    vec3 gold=vec3(1.0,0.75,0.12);
    vec3 yellow=vec3(1.0,0.94,0.45);
    vec3 base=mix(gold,yellow,clamp(N.z*0.5+0.5,0.0,1.0));
    vec3 color=uLighting ? base*(0.60+0.40*diffuse)+vec3(0.15,0.10,0.02)*spec : yellow;
    FragColor=vec4(clamp(color,0.0,1.0),1.0);
}
