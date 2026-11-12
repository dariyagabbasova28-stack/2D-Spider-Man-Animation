#version 330 core
layout(location=0) in vec2 aPos;
layout(location=1) in vec3 aColor;
uniform mat4 uProjection;
uniform mat4 uView;
uniform float uAspect;
uniform bool uOverlay;
out vec3 vColor;
out vec2 vWorld;
void main() {
    vec3 position=vec3(aPos.x*uAspect,aPos.y,0.0);
    gl_Position=uOverlay ? vec4(aPos,0.0,1.0) : uProjection*uView*vec4(position,1.0);
    vColor=aColor;
    vWorld=aPos;
}
