#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
uniform mat4 uProjection,uView;
uniform float uAspect;
uniform vec2 uSunPos;
out vec3 vNormal;
out vec3 vWorld;
void main() {
    // Sphere is rendered in the same 2.5D world as the cartoon background.
    // X is aspect-corrected to keep the sun round in screen space.
    float radius=0.19;
    vec3 center=vec3(uSunPos.x*uAspect,uSunPos.y,0.32);
    vec3 position=center+vec3(aPos.x*uAspect,aPos.y,aPos.z)*radius;
    vWorld=position;
    vNormal=normalize(aNormal);
    gl_Position=uProjection*uView*vec4(position,1.0);
}
