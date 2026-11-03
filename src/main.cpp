#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>
constexpr float PI=3.1415926535f;
struct V {float x,y,r,g,b;};
std::vector<V> verts;
void tri(float x1,float y1,float x2,float y2,float x3,float y3,float r,float g,float b){verts.push_back({x1,y1,r,g,b});verts.push_back({x2,y2,r,g,b});verts.push_back({x3,y3,r,g,b});}
void rect(float x,float y,float w,float h,float r,float g,float b){tri(x,y,x+w,y,x+w,y+h,r,g,b);tri(x,y,x+w,y+h,x,y+h,r,g,b);}
void ellipse(float x,float y,float rx,float ry,float r,float g,float b,int n=40){for(int i=0;i<n;i++){float a=i*2*PI/n,c=(i+1)*2*PI/n;tri(x,y,x+rx*cos(a),y+ry*sin(a),x+rx*cos(c),y+ry*sin(c),r,g,b);}}
void line(float x,float y,float X,float Y,float thick,float r,float g,float b){float dx=X-x,dy=Y-y,len=sqrt(dx*dx+dy*dy);if(len<0.0001f)return;float nx=-dy/len*thick/2,ny=dx/len*thick/2;tri(x+nx,y+ny,x-nx,y-ny,X-nx,Y-ny,r,g,b);tri(x+nx,y+ny,X-nx,Y-ny,X+nx,Y+ny,r,g,b);}
float smooth(float a,float b,float t){t=std::clamp((t-a)/(b-a),0.f,1.f);return t*t*(3-2*t);}
void hero(float x,float y,float s,float angle,float wave){ // local transform of the entire character
 auto pt=[&](float px,float py){px*=s;py*=s;return std::pair<float,float>{x+px*cos(angle)-py*sin(angle),y+px*sin(angle)+py*cos(angle)};};
 auto E=[&](float px,float py,float rx,float ry,float r,float g,float b){auto [cx,cy]=pt(px,py); // use circles transformed as triangles
 for(int i=0;i<40;i++){float a=i*2*PI/40,c=(i+1)*2*PI/40;auto p=pt(px+rx*cos(a),py+ry*sin(a));auto q=pt(px+rx*cos(c),py+ry*sin(c));tri(cx,cy,p.first,p.second,q.first,q.second,r,g,b);}};
 auto L=[&](float ax,float ay,float bx,float by,float w,float r,float g,float b){auto a=pt(ax,ay),c=pt(bx,by);line(a.first,a.second,c.first,c.second,w*s,r,g,b);};
 // silhouette / limbs
 L(-.11f,-.11f,-.27f,-.25f,.10f,.08f,.11f,.24f);L(.11f,-.11f,.27f,-.25f,.10f,.08f,.11f,.24f);
 L(-.11f,-.11f,-.27f,-.25f,.073f,.13f,.34f,.77f);L(.11f,-.11f,.27f,-.25f,.073f,.13f,.34f,.77f);
 L(-.17f,.12f,-.35f,.04f,.10f,.10f,.11f,.22f);L(.17f,.12f,.34f,.06f+wave*.12f,.10f,.10f,.11f,.22f);
 L(-.17f,.12f,-.35f,.04f,.072f,.84f,.13f,.25f);L(.17f,.12f,.34f,.06f+wave*.12f,.072f,.84f,.13f,.25f);
 E(-.35f,.04f,.07f,.065f,.83f,.13f,.25f);E(.34f,.06f+wave*.12f,.07f,.065f,.83f,.13f,.25f);
 E(-.27f,-.25f,.08f,.055f,.83f,.13f,.25f);E(.27f,-.25f,.08f,.055f,.83f,.13f,.25f);
 E(0,.01f,.21f,.25f,.09f,.11f,.23f);E(0,.01f,.18f,.22f,.15f,.35f,.79f);E(0,.07f,.145f,.13f,.86f,.14f,.26f);
 // mask black outline, red fill
 E(0,.40f,.355f,.337f,.09f,.09f,.18f);E(0,.405f,.329f,.31f,.88f,.16f,.28f);
 // web pattern
 for(float k=-.18f;k<=.18f;k+=.12f){L(k,.15f,k*.55f,.69f,.012f,.13f,.08f,.16f);}
 for(float yy=.23f;yy<=.60f;yy+=.115f){L(-.27f,yy,.27f,yy,.012f,.13f,.08f,.16f);}
 E(-.14f,.40f,.155f,.115f,.07f,.07f,.13f);E(.14f,.40f,.155f,.115f,.07f,.07f,.13f);
 E(-.14f,.42f,.123f,.092f,1,1,1);E(.14f,.42f,.123f,.092f,1,1,1);
}
const char* vs=R"(#version 330 core
layout(location=0) in vec2 aPos;layout(location=1) in vec3 aColor;out vec3 vColor;void main(){gl_Position=vec4(aPos,0,1);vColor=aColor;})";
const char* fs=R"(#version 330 core
in vec3 vColor;out vec4 FragColor;void main(){FragColor=vec4(vColor,1);})";
GLuint shader(GLenum type,const char* source){GLuint s=glCreateShader(type);glShaderSource(s,1,&source,nullptr);glCompileShader(s);int ok;glGetShaderiv(s,GL_COMPILE_STATUS,&ok);if(!ok){char msg[1024];glGetShaderInfoLog(s,1024,nullptr,msg);std::cerr<<msg<<'\n';}return s;}
int main(){if(!glfwInit()){std::cerr<<"GLFW initialization failed\n";return 1;}glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);GLFWwindow* win=glfwCreateWindow(1280,720,"Spider Hero - First Demo",nullptr,nullptr);if(!win){glfwTerminate();return 1;}glfwMakeContextCurrent(win);glfwSwapInterval(1);if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){std::cerr<<"GLAD initialization failed\n";return 1;}
 GLuint prog=glCreateProgram(),v=shader(GL_VERTEX_SHADER,vs),f=shader(GL_FRAGMENT_SHADER,fs);glAttachShader(prog,v);glAttachShader(prog,f);glLinkProgram(prog);glDeleteShader(v);glDeleteShader(f);GLuint vao,vbo;glGenVertexArrays(1,&vao);glGenBuffers(1,&vbo);glBindVertexArray(vao);glBindBuffer(GL_ARRAY_BUFFER,vbo);glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(V),(void*)0);glEnableVertexAttribArray(0);glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(V),(void*)(2*sizeof(float)));glEnableVertexAttribArray(1);
 double start=glfwGetTime(),pauseAt=0;bool paused=false,lastSpace=false;while(!glfwWindowShouldClose(win)){glfwPollEvents();if(glfwGetKey(win,GLFW_KEY_ESCAPE)==GLFW_PRESS)glfwSetWindowShouldClose(win,1);bool sp=glfwGetKey(win,GLFW_KEY_SPACE)==GLFW_PRESS;if(sp&&!lastSpace){if(paused){start+=glfwGetTime()-pauseAt;paused=false;}else{pauseAt=glfwGetTime();paused=true;}}lastSpace=sp;if(glfwGetKey(win,GLFW_KEY_R)==GLFW_PRESS){start=glfwGetTime();paused=false;}float t=fmod((float)((paused?pauseAt:glfwGetTime())-start),16.f);verts.clear();
 // pastel sky gradient bands
 for(int i=0;i<32;i++){float yy=-1+2.f*i/32;float a=(float)i/32;rect(-1,yy,2,2.f/32,.91f-.22f*a,.71f+.15f*a,.79f+.16f*a);}
 ellipse(.72f,.66f,.15f,.15f,1,.91f,.69f);
 // distant skyline
 for(int i=0;i<19;i++){float x=-1+i*.115f;float h=.25f+.14f*(.5f+.5f*sin(i*2.2f));rect(x,-.49f,.11f,h,.64f,.61f,.77f);}
 // main buildings and windows
 for(int i=0;i<9;i++){float x=-1+i*.25f;float h=.55f+.24f*(.5f+.5f*sin(i*2.7f));rect(x,-.77f,.23f,h,.27f+.025f*(i%3),.34f,.54f);for(int j=0;j<3;j++)for(int k=0;k<5;k++)rect(x+.025f+j*.065f,-.70f+k*.13f,.032f,.055f,.98f,.81f,.59f);}
 // foreground roof, rail
 rect(-1,-1,2,.27f,.18f,.22f,.36f);rect(-1,-.76f,2,.045f,.12f,.14f,.25f);rect(-1,-.82f,2,.018f,.82f,.40f,.47f);
 // web swing then landing
 float x,y,angle=0,scale=.33f,wave=0;if(t<6){float u=smooth(0,6,t);x=-1.30f+1.5f*u;y=.35f+.24f*sin(u*PI);angle=-.38f+.60f*u;scale=.32f;line(.32f,1.f,x,y+.19f,.009f,.94f,.95f,1);}else if(t<9){float u=smooth(6,9,t);x=.20f+.33f*u;y=.45f-1.07f*u+.13f*sin(u*PI);angle=.2f*(1-u);scale=.32f;}else {x=.53f;y=-.59f+.014f*sin(t*3);scale=.32f;wave=.5f+.5f*sin(t*5);}hero(x,y,scale,angle,wave);
 glClearColor(.9f,.7f,.8f,1);glClear(GL_COLOR_BUFFER_BIT);glUseProgram(prog);glBindVertexArray(vao);glBindBuffer(GL_ARRAY_BUFFER,vbo);glBufferData(GL_ARRAY_BUFFER,verts.size()*sizeof(V),verts.data(),GL_DYNAMIC_DRAW);glDrawArrays(GL_TRIANGLES,0,(GLsizei)verts.size());glfwSwapBuffers(win);}
 glDeleteBuffers(1,&vbo);glDeleteVertexArrays(1,&vao);glDeleteProgram(prog);glfwDestroyWindow(win);glfwTerminate();return 0;}
