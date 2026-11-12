#define NOMINMAX
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <array>
#include <string>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#endif

constexpr float PI = 3.14159265358979323846f;
struct Vertex { float x, y, r, g, b; };
struct Color { float r, g, b; };
std::vector<Vertex> vertices;

void triangle(float ax, float ay, float bx, float by, float cx, float cy, Color c) {
    vertices.push_back({ ax,ay,c.r,c.g,c.b });
    vertices.push_back({ bx,by,c.r,c.g,c.b });
    vertices.push_back({ cx,cy,c.r,c.g,c.b });
}
void rectangle(float x, float y, float w, float h, Color c) {
    triangle(x, y, x + w, y, x + w, y + h, c); triangle(x, y, x + w, y + h, x, y + h, c);
}
void ellipse(float x, float y, float rx, float ry, Color c, int steps = 48) {
    for (int i = 0; i < steps; ++i) {
        float a = 2 * PI * i / steps, b = 2 * PI * (i + 1) / steps;
        triangle(x, y, x + rx * std::cos(a), y + ry * std::sin(a), x + rx * std::cos(b), y + ry * std::sin(b), c);
    }
}
void line(float x, float y, float X, float Y, float width, Color c) {
    float dx = X - x, dy = Y - y, len = std::sqrt(dx * dx + dy * dy);
    if (len < 0.00001f)return;
    float nx = -dy / len * width * .5f, ny = dx / len * width * .5f;
    triangle(x + nx, y + ny, x - nx, y - ny, X - nx, Y - ny, c);
    triangle(x + nx, y + ny, X - nx, Y - ny, X + nx, Y + ny, c);
}
float clamp01(float t) { return std::clamp(t, 0.f, 1.f); }
float ease(float t) { t = clamp01(t); return t * t * (3.f - 2.f * t); }
float phase(float t, float start, float end) { return ease((t - start) / (end - start)); }
float mix(float a, float b, float t) { return a + (b - a) * t; }
struct Point { float x, y; };
Point blend(Point a, Point b, float t) { return { mix(a.x,b.x,t),mix(a.y,b.y,t) }; }

const Color outline{ .105f,.075f,.18f };
const Color red{ .90f,.16f,.27f };
const Color blue{ .14f,.36f,.79f };
const Color white{ .99f,.98f,.97f };
const Color skin{ .96f,.71f,.56f };

void sky() {
    for (int i = 0; i < 64; i++) {
        float u = (float)i / 64, y = -1.f + 2.f * u;
        rectangle(-1, y, 2, 2.f / 64.f, { mix(.99f,.48f,u),mix(.70f,.67f,u),mix(.68f,.91f,u) });
    }
    for (int i = 0; i < 15; i++) {
        float x = -1.03f + i * .15f, h = .22f + .19f * (.5f + .5f * std::sin(i * 2.31f));
        rectangle(x, -.58f, .145f, h, { .65f,.58f,.72f });
    }
    for (int i = 0; i < 10; i++) {
        float x = -1.02f + i * .235f, h = .52f + .17f * (.5f + .5f * std::sin(i * 2.4f));
        rectangle(x, -.81f, .218f, h, { .31f + .02f * (i % 3),.36f,.55f });
        for (int row = 0; row < 4; row++)for (int col = 0; col < 3; col++) {
            float yy = -.74f + row * .13f;
            if (yy < -.81f + h - .05f)rectangle(x + .025f + col * .062f, yy, .032f, .058f, { .99f,.82f,.59f });
        }
    }
    rectangle(-1, -1, 2, .25f, { .19f,.23f,.39f });
    rectangle(-1, -.76f, 2, .048f, { .12f,.16f,.28f });
    rectangle(-1, -.81f, 2, .015f, { .87f,.44f,.52f });
}

void balloon(Point p, float size) {
    line(p.x, p.y - .10f * size, p.x + .025f * size, p.y - .37f * size, .009f * size, { .34f,.26f,.35f });
    ellipse(p.x, p.y, .105f * size, .132f * size, outline);
    ellipse(p.x, p.y + .006f * size, .094f * size, .12f * size, { .98f,.20f,.33f });
    ellipse(p.x - .031f * size, p.y + .05f * size, .018f * size, .032f * size, { 1.f,.66f,.66f });
    triangle(p.x - .019f * size, p.y - .119f * size, p.x + .019f * size, p.y - .119f * size, p.x, p.y - .151f * size, red);
}

void girl(float x, float y, float s, bool happy, float armRaise) {
    auto E = [&](float px, float py, float rx, float ry, Color c) {ellipse(x + px * s, y + py * s, rx * s, ry * s, c); };
    auto L = [&](float ax, float ay, float bx, float by, float w, Color c) {line(x + ax * s, y + ay * s, x + bx * s, y + by * s, w * s, c); };
    L(-.08f, -.18f, -.09f, -.36f, .08f, outline); L(.08f, -.18f, .09f, -.36f, .08f, outline);
    L(-.08f, -.18f, -.09f, -.36f, .055f, { .42f,.33f,.67f }); L(.08f, -.18f, .09f, -.36f, .055f, { .42f,.33f,.67f });
    E(-.09f, -.37f, .07f, .035f, outline); E(.09f, -.37f, .07f, .035f, outline);
    triangle(x - .15f * s, y - .27f * s, x + .15f * s, y - .27f * s, x, y + .10f * s, { .94f,.47f,.65f });
    L(-.10f, .045f, -.20f, -.08f, .06f, skin);
    L(.10f, .045f, .22f, -.05f + armRaise * .24f, .06f, skin);
    E(-.20f, -.08f, .042f, .043f, skin); E(.22f, -.05f + armRaise * .24f, .042f, .043f, skin);
    E(-.19f, .26f, .085f, .12f, { .27f,.17f,.24f }); E(.19f, .26f, .085f, .12f, { .27f,.17f,.24f });
    E(0, .28f, .23f, .245f, outline); E(0, .29f, .215f, .225f, { .30f,.19f,.26f });
    E(0, .235f, .187f, .184f, skin);
    E(-.065f, .27f, .017f, .024f, outline); E(.065f, .27f, .017f, .024f, outline);
    if (happy) {
        for (int i = 0; i < 12; i++) {
            float a = PI * 1.12f + PI * .76f * i / 11;
            float b = PI * 1.12f + PI * .76f * (i + 1) / 11;
            L(.0f + .065f * std::cos(a), .15f + .048f * std::sin(a), .065f * std::cos(b), .15f + .048f * std::sin(b), .009f, outline);
        }
    }
    else {
        L(-.055f, .14f, .055f, .14f, .012f, outline);
        E(-.085f, .20f, .012f, .038f, { .58f,.83f,1.f });
    }
}

void hero(float x, float y, float s, float angle, float wave, float crouch) {
    const Color darkBlue{ .075f,.17f,.43f }, lightBlue{ .24f,.48f,.88f };
    const Color darkRed{ .57f,.065f,.14f }, highlight{ 1.f,.34f,.38f };
    float ca = std::cos(angle), sa = std::sin(angle);
    auto pt = [&](float px, float py)->Point {return { x + s * (px * ca - py * sa),y + s * (px * sa + py * ca) }; };
    auto E = [&](float px, float py, float rx, float ry, Color c) {
        Point center = pt(px, py);
        for (int i = 0; i < 48; i++) {
            float a = 2 * PI * i / 48, b = 2 * PI * (i + 1) / 48;
            Point p = pt(px + rx * std::cos(a), py + ry * std::sin(a));
            Point q = pt(px + rx * std::cos(b), py + ry * std::sin(b));
            triangle(center.x, center.y, p.x, p.y, q.x, q.y, c);
        }
        };
    auto L = [&](float ax, float ay, float bx, float by, float w, Color c) {
        Point a = pt(ax, ay), b = pt(bx, by); line(a.x, a.y, b.x, b.y, w * s, c);
        };
    auto T = [&](Point a, Point b, Point c, Color color) {
        a = pt(a.x, a.y); b = pt(b.x, b.y); c = pt(c.x, c.y);
        triangle(a.x, a.y, b.x, b.y, c.x, c.y, color);
        };
    // Articulated legs: thigh, knee, shin and boots.
    for (int side : {-1, 1}) {
        float d = (float)side;
        Point hip{ d * .095f,-.18f };
        Point knee{ d * (.15f + .065f * crouch),-.43f + .10f * crouch };
        Point ankle{ d * (.17f + .11f * crouch),-.68f + .20f * crouch };
        L(hip.x, hip.y, knee.x, knee.y, .145f, outline);
        L(hip.x, hip.y, knee.x, knee.y, .115f, darkBlue);
        L(hip.x, hip.y + .015f, knee.x, knee.y + .03f, .032f, lightBlue);
        E(knee.x, knee.y, .067f, .068f, outline);
        E(knee.x, knee.y, .051f, .052f, blue);
        L(knee.x, knee.y, ankle.x, ankle.y, .115f, outline);
        L(knee.x, knee.y, ankle.x, ankle.y, .087f, red);
        L(knee.x, knee.y - .035f, ankle.x, ankle.y, .018f, darkRed);
        E(ankle.x + d * .045f, ankle.y - .012f, .116f, .060f, outline);
        E(ankle.x + d * .045f, ankle.y - .003f, .101f, .046f, red);
        L(ankle.x - d * .015f, ankle.y + .015f, ankle.x + d * .09f, ankle.y + .015f, .010f, darkRed);
    }
    // Narrow waist, shoulders and chest panels.
    E(0, -.115f, .155f, .17f, outline);
    E(0, -.11f, .137f, .155f, darkBlue);
    E(-.11f, .08f, .105f, .185f, outline);
    E(.11f, .08f, .105f, .185f, outline);
    E(-.105f, .08f, .086f, .165f, blue);
    E(.105f, .08f, .086f, .165f, blue);
    E(0, .10f, .155f, .18f, red);
    E(0, .175f, .105f, .072f, highlight);
    // Small spider chest emblem with eight legs.
    E(0, .095f, .018f, .038f, outline);
    E(0, .135f, .014f, .016f, outline);
    for (int side : {-1, 1}) {
        float d = (float)side;
        for (int i = 0; i < 4; i++) {
            float yy = .13f - i * .025f;
            L(d * .012f, yy, d * (.043f + i * .008f), yy + .025f - i * .012f, .010f, outline);
            L(d * (.043f + i * .008f), yy + .025f - i * .012f, d * (.066f + i * .008f), yy + .009f - i * .019f, .008f, outline);
        }
    }
    // Two-segment arms; right arm raises when waving.
    for (int side : {-1, 1}) {
        float d = (float)side;
        Point shoulder{ d * .18f,.17f };
        Point elbow{ d * .31f,.015f + (side == 1 ? .15f * wave : 0.f) };
        Point hand{ d * .42f,-.085f + (side == 1 ? .36f * wave : 0.f) };
        L(shoulder.x, shoulder.y, elbow.x, elbow.y, .125f, outline);
        L(shoulder.x, shoulder.y, elbow.x, elbow.y, .096f, red);
        L(shoulder.x - d * .012f, shoulder.y, elbow.x - d * .01f, elbow.y, .017f, highlight);
        E(elbow.x, elbow.y, .061f, .06f, outline);
        E(elbow.x, elbow.y, .047f, .047f, red);
        L(elbow.x, elbow.y, hand.x, hand.y, .105f, outline);
        L(elbow.x, elbow.y, hand.x, hand.y, .079f, darkRed);
        L(elbow.x, elbow.y, hand.x, hand.y, .048f, red);
        E(hand.x, hand.y, .070f, .070f, outline);
        E(hand.x, hand.y, .058f, .058f, red);
        for (int i = 0; i < 3; i++) {
            float yy = .025f - i * .035f;
            L(d * (.32f + i * .025f), yy + (side == 1 ? .18f * wave : 0.f),
                d * (.34f + i * .025f), yy - .018f + (side == 1 ? .18f * wave : 0.f), .008f, darkRed);
        }
    }
    // Neck and elongated mask (smaller than the original cartoon head).
    E(0, .277f, .070f, .074f, outline);
    E(0, .28f, .055f, .065f, darkRed);
    E(0, .445f, .245f, .265f, outline);
    E(0, .449f, .228f, .249f, red);
    E(-.065f, .535f, .110f, .118f, highlight);
    // Web pattern follows the mask silhouette.
    for (int i = -3; i <= 3; i++) {
        float xx = i * .052f;
        L(0, .67f, xx, .26f + .035f * std::abs(i), .008f, darkRed);
    }
    for (int row = 0; row < 4; row++) {
        float yy = .32f + row * .082f;
        float ratio = (yy - .449f) / .249f;
        float half = .218f * std::sqrt((std::max)(0.f, 1.f - ratio * ratio));
        for (int i = 0; i < 10; i++) {
            float a = -half + 2 * half * i / 10, b = -half + 2 * half * (i + 1) / 10;
            float bow = (i % 2 == 0 ? .012f : -.012f);
            L(a, yy, b, yy + bow, .007f, darkRed);
        }
    }
    // Angular white eye lenses and dark borders.
    for (int side : {-1, 1}) {
        float d = (float)side;
        T({ d * .025f,.48f }, { d * .198f,.532f }, { d * .166f,.402f }, outline);
        T({ d * .025f,.48f }, { d * .166f,.402f }, { d * .07f,.399f }, outline);
        T({ d * .044f,.474f }, { d * .177f,.507f }, { d * .149f,.419f }, white);
        T({ d * .044f,.474f }, { d * .149f,.419f }, { d * .077f,.416f }, white);
    }
}


// Scene 2: street rescue, using the same geometry and Spider-Man model.
void street() {
    for (int i = 0; i < 52; i++) {
        float u = float(i) / 52.f;
        rectangle(-1.f, -1.f + 2.f * u, 2.f, 2.f / 52.f,
            { mix(.28f,.65f,u),mix(.33f,.69f,u),mix(.51f,.84f,u) });
    }
    for (int i = 0; i < 9; i++) {
        float x = -1.1f + i * .27f;
        float h = .72f + .13f * std::sin(i * 2.3f);
        rectangle(x, -.69f, .255f, h, { .21f + .025f * (i % 3),.27f,.44f });
        rectangle(x + .012f, -.69f, .012f, h, { .13f,.18f,.33f });
        for (int row = 0; row < 5; row++)for (int col = 0; col < 3; col++) {
            float wy = -.58f + row * .13f;
            if (wy < -.69f + h - .07f)rectangle(x + .038f + col * .064f, wy, .035f, .062f,
                (row + col + i) % 4 == 0 ? Color{ .33f,.45f,.64f } : Color{ .99f,.79f,.49f });
        }
    }
    rectangle(-1.f, -1.f, 2.f, .27f, { .21f,.24f,.32f });
    rectangle(-1.f, -.75f, 2.f, .048f, { .40f,.43f,.51f });
    rectangle(-1.f, -.82f, 2.f, .012f, { .85f,.71f,.54f });
    for (int i = 0; i < 8; i++)rectangle(-.92f + i * .27f, -.94f, .12f, .012f, { .79f,.76f,.65f });
    // Lamp and alley sign.
    line(.83f, -.75f, .83f, .12f, .024f, { .12f,.15f,.25f });
    line(.83f, .12f, .67f, .12f, .02f, { .12f,.15f,.25f });
    ellipse(.67f, .095f, .057f, .028f, { 1.f,.85f,.53f });
}
void bag(Point p, float s) {
    Color brown{ .59f,.29f,.16f };
    rectangle(p.x - .09f * s, p.y - .075f * s, .18f * s, .15f * s, outline);
    rectangle(p.x - .078f * s, p.y - .063f * s, .156f * s, .126f * s, brown);
    ellipse(p.x, p.y + .07f * s, .048f * s, .055f * s, outline);
    ellipse(p.x, p.y + .065f * s, .027f * s, .039f * s, { .73f,.45f,.26f });
    rectangle(p.x - .012f * s, p.y - .025f * s, .024f * s, .055f * s, { .94f,.74f,.36f });
}
void person(float x, float y, float s, bool frightened, float arm) {
    auto E = [&](float px, float py, float rx, float ry, Color c) {ellipse(x + px * s, y + py * s, rx * s, ry * s, c); };
    auto L = [&](float ax, float ay, float bx, float by, float w, Color c) {line(x + ax * s, y + ay * s, x + bx * s, y + by * s, w * s, c); };
    L(-.09f, -.13f, -.12f, -.39f, .085f, outline); L(.09f, -.13f, .12f, -.39f, .085f, outline);
    L(-.09f, -.13f, -.12f, -.39f, .061f, { .18f,.23f,.43f }); L(.09f, -.13f, .12f, -.39f, .061f, { .18f,.23f,.43f });
    E(-.13f, -.40f, .077f, .038f, outline); E(.13f, -.40f, .077f, .038f, outline);
    E(0, .015f, .19f, .23f, outline); E(0, .02f, .17f, .21f, { .32f,.68f,.65f });
    L(-.15f, .12f, -.29f, -.05f, .083f, skin); L(.15f, .12f, .28f, .01f + arm * .21f, .083f, skin);
    E(-.29f, -.05f, .046f, .047f, skin); E(.28f, .01f + arm * .21f, .046f, .047f, skin);
    E(0, .37f, .20f, .215f, outline); E(0, .37f, .184f, .198f, skin);
    E(0, .51f, .19f, .075f, { .22f,.15f,.22f });
    E(-.064f, .39f, .014f, .018f, outline); E(.064f, .39f, .014f, .018f, outline);
    if (frightened)E(0, .29f, .018f, .031f, outline);
    else line(-.045f * s + x, .29f * s + y, .045f * s + x, .29f * s + y, .012f * s, outline);
}
void thief(float x, float y, float s, float stride, bool tied) {
    auto E = [&](float px, float py, float rx, float ry, Color c) {ellipse(x + px * s, y + py * s, rx * s, ry * s, c); };
    auto L = [&](float ax, float ay, float bx, float by, float w, Color c) {line(x + ax * s, y + ay * s, x + bx * s, y + by * s, w * s, c); };
    Color coat{ .36f,.33f,.42f };
    L(-.08f, -.12f, -.12f + stride, -.38f, .11f, outline);
    L(.08f, -.12f, .12f - stride, -.38f, .11f, outline);
    L(-.08f, -.12f, -.12f + stride, -.38f, .078f, { .17f,.19f,.29f });
    L(.08f, -.12f, .12f - stride, -.38f, .078f, { .17f,.19f,.29f });
    E(0, .015f, .20f, .24f, outline); E(0, .02f, .18f, .22f, coat);
    L(-.15f, .12f, -.29f, -.03f, .11f, outline);
    L(.15f, .12f, .30f, .02f, .11f, outline);
    L(-.15f, .12f, -.29f, -.03f, .075f, coat);
    L(.15f, .12f, .30f, .02f, .075f, coat);
    E(0, .38f, .21f, .215f, outline); E(0, .38f, .19f, .195f, skin);
    rectangle(x - .19f * s, y + .43f * s, .38f * s, .095f * s, { .14f,.17f,.24f });
    rectangle(x - .15f * s, y + .34f * s, .30f * s, .055f * s, outline);
    E(-.07f, .385f, .017f, .016f, white); E(.07f, .385f, .017f, .016f, white);
    if (tied) {
        for (int i = 0; i < 5; i++)L(-.23f, .16f - i * .085f, .23f, .16f - i * .085f, .016f, white);
        for (int i = 0; i < 3; i++)L(-.17f + i * .17f, .23f, -.17f + i * .17f, -.24f, .012f, white);
    }
}
void sceneOne(float t) {
    sky();
    bool happy = t >= 14.f;
    girl(-.62f, -.59f, .52f, happy, phase(t, 14, 16));
    Point balloonPos;
    if (t < 4.f) { float u = phase(t, 0, 4); balloonPos = { -.52f + .15f * u,-.15f + .55f * u }; }
    else if (t < 11.f) { float u = phase(t, 4, 11); balloonPos = { -.37f + .16f * u,.40f + .09f * std::sin(u * PI) }; }
    else if (t < 15.f) { float u = phase(t, 11, 15); balloonPos = blend({ -.21f,.40f }, { -.47f,-.23f }, u); }
    else balloonPos = { -.47f,-.23f + .009f * std::sin(t * 3) };
    balloon(balloonPos, .85f);
    float hx, hy, angle = 0, wave = 0, crouch = 0;
    if (t < 4.f) { hx = -1.55f; hy = .20f; }
    else if (t < 8.f) { float u = phase(t, 4, 8); hx = mix(-1.3f, .22f, u); hy = .17f + .32f * std::sin(PI * u); angle = mix(-.30f, .16f, u); }
    else if (t < 11.f) { float u = phase(t, 8, 11); hx = mix(.22f, -.12f, u); hy = mix(.22f, .30f, u); angle = mix(.16f, -.10f, u); }
    else if (t < 14.f) { float u = phase(t, 11, 14); hx = mix(-.12f, -.08f, u); hy = mix(.30f, -.58f, u); crouch = .6f * std::sin(PI * u); }
    else { hx = -.08f; hy = -.58f; wave = phase(t, 15, 16) * (.5f + .5f * std::sin((t - 16) * 7)); crouch = .12f * phase(t, 18, 19); }
    if (t >= 4.f && t < 8.f)line(.18f, 1.f, hx, hy + .16f, .007f, white);
    if (t >= 8.f && t < 11.f)line(hx - .08f, hy + .08f, balloonPos.x, balloonPos.y - .11f, .008f, white);
    hero(hx, hy, .34f, angle, wave, crouch);
}
void sceneTwo(float t) {
    street();
    // A thief runs away with the bag; Spider-Man catches and restrains them.
    float thiefX = t < 11.f ? mix(-.10f, .46f, phase(t, 0, 11)) : .46f;
    bool tied = t >= 17.f;
    float stride = t < 11.f ? .045f * std::sin(t * 12.f) : 0.f;
    thief(thiefX, -.57f, .39f, stride, tied);
    float personArm = t > 23.f ? phase(t, 23, 26) : 0.f;
    person(-.69f, -.57f, .39f, t < 24.f, personArm);
    Point bagPos;
    if (t < 12.f)bagPos = { thiefX + .14f,-.56f };
    else if (t < 23.f)bagPos = { .60f,-.56f };
    else if (t < 28.f)bagPos = blend({ .60f,-.56f }, { -.47f,-.53f }, phase(t, 23, 28));
    else bagPos = { -.47f,-.53f };
    bag(bagPos, .80f);
    float hx = -1.5f, hy = .4f, ang = 0.f, wave = 0.f, crouch = 0.f;
    if (t < 6.f) { hx = -1.5f; }
    else if (t < 11.f) { float u = phase(t, 6, 11); hx = mix(-1.1f, .13f, u); hy = .23f + .27f * std::sin(u * PI); ang = mix(-.4f, .05f, u); }
    else if (t < 15.f) { float u = phase(t, 11, 15); hx = mix(.13f, .22f, u); hy = mix(.23f, -.57f, u); crouch = .7f * std::sin(u * PI); }
    else if (t < 23.f) { hx = .22f; hy = -.57f; wave = .25f; }
    else if (t < 29.f) { float u = phase(t, 23, 29); hx = mix(.22f, -.27f, u); hy = -.57f + .04f * std::sin(u * PI); }
    else if (t < 33.f) { hx = -.27f; hy = -.57f; wave = .5f + .5f * std::sin((t - 29.f) * 6.f); }
    else { float u = phase(t, 33, 35); hx = mix(-.27f, 1.5f, u); hy = mix(-.57f, .6f, u); ang = .4f * u; }
    if (t >= 6.f && t < 11.f)line(-.15f, 1.f, hx, hy + .14f, .008f, white);
    if (t >= 15.f && t < 17.f) {
        float u = phase(t, 15, 17);
        line(hx + .10f, hy + .10f, mix(hx + .10f, thiefX - .10f, u), mix(hy + .10f, -.51f, u), .008f, white);
    }
    hero(hx, hy, .34f, ang, wave, crouch);
}



// Comedy interlude: a pigeon interrupts a dramatic rooftop entrance (8 seconds).
void pigeon(float x, float y, float scale, float time) {
    const Color feather{ .72f,.77f,.85f }, wing{ .49f,.57f,.70f }, beak{ 1.f,.73f,.24f };
    float flap = std::sin(time * 18.f);
    ellipse(x, y, .105f * scale, .065f * scale, outline);
    ellipse(x, y + .008f * scale, .092f * scale, .053f * scale, feather);
    // Wings flap vertically, making the bird recognizable even at small sizes.
    triangle(x - .025f * scale, y, x - .105f * scale, y + (.10f + .09f * flap) * scale,
             x + .035f * scale, y + .025f * scale, wing);
    triangle(x - .01f * scale, y, x + .065f * scale, y + (.13f + .085f * flap) * scale,
             x + .065f * scale, y + .008f * scale, feather);
    ellipse(x + .075f * scale, y + .044f * scale, .049f * scale, .048f * scale, feather);
    ellipse(x + .09f * scale, y + .055f * scale, .010f * scale, .012f * scale, outline);
    triangle(x + .115f * scale, y + .039f * scale, x + .165f * scale, y + .020f * scale,
             x + .115f * scale, y + .013f * scale, beak);
    triangle(x - .085f * scale, y, x - .17f * scale, y + .055f * scale,
             x - .14f * scale, y - .03f * scale, wing);
}

void comedyScene(float t) {
    sky();
    // Rooftop antenna and an obvious swinging anchor.
    rectangle(.64f, -.75f, .035f, .93f, outline);
    rectangle(.655f, .12f, .18f, .025f, { .79f,.84f,.91f });
    ellipse(.66f, .19f, .043f, .043f, red);
    line(.66f, .17f, .48f, .34f, .012f, outline);
    line(.66f, .17f, .84f, .34f, .012f, outline);
    // Pigeon approaches from the right and then flees in panic.
    float bx = t < 4.8f ? mix(1.35f, -.13f, phase(t, 1.0f, 4.8f))
                         : mix(-.13f, -1.5f, phase(t, 4.8f, 7.0f));
    float by = .12f + .07f * std::sin(t * 9.f);
    pigeon(bx, by, 1.05f, t);

    float hx = -.25f, hy = -.54f, ang = 0.f, wave = 0.f, crouch = 0.f;
    if (t < 3.7f) {
        float u = phase(t, 0.f, 3.7f);
        hx = mix(-1.3f, -.18f, u);
        hy = mix(.12f, .22f, u) + .12f * std::sin(PI * u);
        ang = -.25f + .25f * u;
        line(.66f, .19f, hx, hy + .16f, .009f, white);
    } else if (t < 5.1f) {
        hx = -.18f + .04f * std::sin(t * 18.f);
        hy = .22f;
        ang = .16f * std::sin(t * 16.f);
        line(.66f, .19f, hx, hy + .16f, .009f, white);
        // Surprise marks around Spider-Man's head.
        for (int i = 0; i < 3; ++i)
            line(hx - .16f + i * .10f, hy + .39f, hx - .19f + i * .10f,
                 hy + .48f, .012f, { 1.f,.91f,.48f });
    } else if (t < 8.7f) {
        float u = (t - 5.1f) / 3.6f;
        float a = u * 4.f * PI;
        hx = .66f - .55f * std::cos(a);
        hy = .19f - .43f * std::sin(a);
        ang = -a;
        crouch = .35f;
        line(.66f, .19f, hx, hy + .09f, .010f, white);
        for (int i = 0; i < 5; ++i) {
            float aa = a - i * .19f;
            float xx = .66f - .55f * std::cos(aa);
            float yy = .19f - .43f * std::sin(aa);
            line(xx - .045f, yy, xx + .045f, yy, .006f, { .98f,.85f,.70f });
        }
    } else if (t < 10.1f) {
        float u = phase(t, 8.7f, 10.1f);
        hx = mix(.11f, -.30f, u);
        hy = mix(.19f, -.54f, u) + .13f * std::sin(PI * u);
        ang = -2.f * PI * (1.f - u);
        crouch = .7f * std::sin(PI * u);
    } else {
        hx = -.30f; hy = -.54f;
        crouch = .6f * (1.f - phase(t, 10.1f, 11.0f));
        wave = phase(t, 10.7f, 11.6f);
        // Little impact rays on landing.
        if (t < 10.8f) {
            line(hx - .24f, -.75f, hx - .35f, -.67f, .012f, white);
            line(hx + .24f, -.75f, hx + .35f, -.67f, .012f, white);
        }
    }
    hero(hx, hy, .36f, ang, wave, crouch);
}

// Scene 3: short web-swinging transition (7 seconds).
void chaseCity(float t) {
    const Color skyTop{ .12f,.19f,.39f }, skyBottom{ .69f,.39f,.58f };
    for (int i = 0; i < 64; i++) {
        float u = float(i) / 64.f;
        rectangle(-1.f, -1.f + 2.f * u, 2.f, 2.f / 64.f,
            { mix(skyBottom.r,skyTop.r,u),mix(skyBottom.g,skyTop.g,u),mix(skyBottom.b,skyTop.b,u) });
    }
    // Parallax: distant towers move slower than foreground towers.
    for (int layer = 0; layer < 2; layer++) {
        float speed = layer == 0 ? .13f : .38f;
        float spacing = layer == 0 ? .31f : .43f;
        float offset = std::fmod(t * speed, spacing);
        for (int i = -2; i < 10; i++) {
            float x = -1.2f + i * spacing - offset;
            float h = (layer == 0 ? .70f : 1.08f) + .22f * (.5f + .5f * std::sin(i * 2.37f + layer));
            float bottom = layer == 0 ? -.80f : -1.f;
            Color c = layer == 0 ? Color{ .35f,.35f,.55f } : Color{ .15f,.20f,.39f };
            rectangle(x, bottom, spacing * .83f, h, c);
            if (layer == 1) {
                rectangle(x, bottom + h - .025f, spacing * .83f, .025f, { .10f,.13f,.28f });
                for (int row = 0; row < 7; row++)for (int col = 0; col < 3; col++) {
                    float wx = x + .035f + col * spacing * .23f;
                    float wy = bottom + .10f + row * .135f;
                    if (wy < bottom + h - .07f)
                        rectangle(wx, wy, .042f, .060f, ((row + col + i + 100) % 4 == 0) ? Color{ .30f,.41f,.60f } : Color{ .98f,.75f,.46f });
                }
            }
        }
    }
    // Speed lines create a sense of movement.
    for (int i = 0; i < 13; i++) {
        float xx = std::fmod(i * .263f - t * .9f + 50.f, 2.4f) - 1.2f;
        float yy = .05f + .055f * i;
        line(xx, yy, xx + .12f, yy, .003f, { .83f,.74f,.84f });
    }
    float hx = -.13f + .19f * std::sin(t * .56f);
    float hy = .16f + .24f * std::sin(t * 1.35f);
    float angle = -.24f + .33f * std::cos(t * 1.35f);
    // Web anchors alternate between left and right rooftops.
    float cycle = std::fmod(t, 5.f);
    float anchorX = cycle < 2.5f ? -.75f : .76f;
    if (t < 5.5f) {
        line(anchorX, 1.f, hx, hy + .19f, .009f, white);
        ellipse(anchorX, 1.f, .015f, .015f, white);
    }
    float crouch = .23f + .20f * std::sin(t * 2.7f);
    if (t >= 5.5f) {
        float u = phase(t, 5.5f, 7.f);
        hx = mix(hx, 1.35f, u); hy = mix(hy, .85f, u);
        angle = mix(angle, .45f, u);
    }
    hero(hx, hy, .39f, angle, .30f, crouch);
}


// Seven-segment-style vector lettering, drawn entirely with OpenGL triangles.
void endLetter(char ch, float x, float y, float z) {
    const Color ink{.12f,.17f,.29f};
    auto stroke=[&](float a,float b,float c,float d){line(x+a*z,y+b*z,x+c*z,y+d*z,.10f*z,ink);};
    if(ch=='T'){stroke(0,1,1,1);stroke(.5f,1,.5f,0);}
    if(ch=='H'){stroke(0,0,0,1);stroke(1,0,1,1);stroke(0,.5f,1,.5f);}
    if(ch=='E'){stroke(1,1,0,1);stroke(0,1,0,0);stroke(0,.5f,.85f,.5f);stroke(0,0,1,0);}
    if(ch=='N'){stroke(0,0,0,1);stroke(0,1,1,0);stroke(1,0,1,1);}
    if(ch=='D'){stroke(0,0,0,1);stroke(0,1,.65f,1);stroke(.65f,1,1,.78f);stroke(1,.78f,1,.22f);stroke(1,.22f,.65f,0);stroke(.65f,0,0,0);}
}
void endTitle(){
    const char* msg="THE END";
    float unit=.135f, gap=.045f, left=-.69f;
    for(int i=0;msg[i];++i) if(msg[i]!=' ')endLetter(msg[i],left+i*(unit+gap),-.08f,unit);
}
void webToCamera(float u){
    // Web expands from the hero's extended hand to fill the camera view.
    const Color webInk{.73f,.77f,.83f};
    const float cx=-.04f,cy=.05f;
    float radius=.04f+2.65f*phase(u,0.f,1.f);
    // White connected web strands first; then a solid white frame at the end.
    for(int ring=1;ring<=7;++ring){
        float r=radius*ring/7.f;
        for(int j=0;j<24;++j){
            float a=2*PI*j/24.f,b=2*PI*(j+1)/24.f;
            line(cx+r*std::cos(a),cy+r*std::sin(a),cx+r*std::cos(b),cy+r*std::sin(b),.009f+u*.012f,white);
        }
    }
    for(int j=0;j<24;++j){
        float a=2*PI*j/24.f;
        line(cx,cy,cx+radius*std::cos(a),cy+radius*std::sin(a),.009f+u*.008f,white);
    }
    if(u>.65f){
        // Solid white closing wipe grows across the entire viewport.
        float wipe=phase(u,.65f,1.f);
        ellipse(cx,cy,2.9f*wipe,2.9f*wipe,white,96);
    }
}

// Scene 5: sunset rooftop and camera-facing web finale (34 seconds).
void sceneFour(float t) {
    const Color duskLow{ .91f,.44f,.43f }, duskHigh{ .065f,.095f,.24f };
    float night = phase(t, 3.f, 22.f);
    for (int i = 0; i < 64; ++i) {
        float u = float(i) / 64.f;
        Color c = { mix(duskLow.r,duskHigh.r,u),mix(duskLow.g,duskHigh.g,u),mix(duskLow.b,duskHigh.b,u) };
        c.r = mix(c.r, c.r * .54f, night); c.g = mix(c.g, c.g * .64f, night); c.b = mix(c.b, c.b * .83f, night);
        rectangle(-1.f, -1.f + 2.f * u, 2.f, 2.f / 64.f, c);
    }
    for (int i = 0; i < 24; ++i) {
        float x = -.98f + float((i * 47) % 97) / 49.f;
        float y = .12f + float((i * 31) % 79) / 100.f;
        float glow = night * phase(t, 8.f, 18.f);
        if (glow > .03f)ellipse(x, y, .0035f, .0035f, { glow,glow,glow });
    }
    for (int layer = 0; layer < 2; ++layer) {
        for (int i = 0; i < 13; ++i) {
            float x = -1.08f + i * .174f + (layer ? -.06f : 0.f);
            float h = (layer ? .47f : .31f) + .18f * (.5f + .5f * std::sin(i * 2.17f + layer));
            float bottom = layer ? -.75f : -.57f;
            rectangle(x, bottom, .158f, h, layer ? Color{ .12f,.16f,.30f } : Color{ .28f,.26f,.40f });
            for (int row = 0; row < 5; ++row)for (int col = 0; col < 2; ++col) {
                float wy = bottom + .07f + row * .11f;
                if (wy < bottom + h - .06f) {
                    bool lit = ((i * 7 + row * 3 + col) % 5) != 0;
                    Color wc = lit ? Color{ mix(.50f,1.f,night),mix(.49f,.80f,night),mix(.53f,.42f,night) } : Color{ .25f,.30f,.42f };
                    rectangle(x + .03f + col * .066f, wy, .027f, .043f, wc);
                }
            }
        }
    }
    rectangle(-1.f, -1.f, 2.f, .30f, { .095f,.12f,.23f });
    rectangle(-1.f, -.72f, 2.f, .045f, { .055f,.075f,.16f });
    rectangle(-1.f, -.78f, 2.f, .015f, { .54f,.27f,.38f });
    for (int i = 0; i < 9; ++i) {
        float x = -.96f + i * .24f;
        rectangle(x, -.71f, .012f, .15f, { .09f,.12f,.22f });
    }
    rectangle(-1.f, -.58f, 2.f, .012f, { .09f,.12f,.22f });
    float hx, hy, ang = 0.f, wave = 0.f, crouch = 0.f;
    if (t < 4.f) {
        float u = phase(t, 0.f, 4.f);
        hx = mix(-1.32f, -.30f, u); hy = mix(.30f, -.56f, u) + .12f * std::sin(PI * u);
        ang = mix(-.28f, 0.f, u); crouch = .42f * std::sin(PI * u);
        if (t < 3.f)line(-.83f, 1.f, hx, hy + .17f, .008f, white);
    }
    else {
        // Spider-Man remains on the same rooftop for the closing shot.
        hx = -.30f; hy = -.55f + .006f * std::sin(t * 2.f);
        wave = (t >= 24.f) ? phase(t,24.f,26.f) : phase(t,14.f,16.f)*(.25f+.25f*std::sin(t*4.f));
    }
    hero(hx, hy, .36f, ang, wave, crouch);
    if(t>=26.f && t<29.f){
        // The red glove grows toward the audience: a simple forced-perspective pose.
        float reach=phase(t,26.f,27.f);
        line(-.17f,-.48f,-.04f,-.12f,.035f+.07f*reach,outline);
        line(-.17f,-.48f,-.04f,-.12f,.025f+.055f*reach,red);
        ellipse(-.04f,-.12f,.035f+.085f*reach,.032f+.075f*reach,outline);
        ellipse(-.04f,-.12f,.028f+.072f*reach,.026f+.063f*reach,red);
    }
    if(t>=27.f && t<29.f)webToCamera((t-27.f)/2.f);
    if(t>=29.f){
        rectangle(-1.f,-1.f,2.f,2.f,white);
        endTitle();
    }
}

std::string readShader(const std::string& path) {
    std::ifstream file(path);
    if (!file) { std::cerr << "Missing shader: " << path << "\\n"; return {}; }
    std::ostringstream out; out << file.rdbuf(); return out.str();
}
struct Mat4 { float v[16] = {}; };
Mat4 identity() { Mat4 m; m.v[0]=m.v[5]=m.v[10]=m.v[15]=1.f; return m; }
Mat4 perspective(float fov, float aspect, float nearZ, float farZ) {
    Mat4 m; float f=1.f/std::tan(fov*.5f);
    m.v[0]=f/aspect; m.v[5]=f;
    m.v[10]=(farZ+nearZ)/(nearZ-farZ);
    m.v[11]=-1.f; m.v[14]=(2.f*farZ*nearZ)/(nearZ-farZ);
    return m;
}
Mat4 cameraView(float x, float y, float z) {
    Mat4 m=identity(); m.v[12]=-x; m.v[13]=-y; m.v[14]=-z; return m;
}
void drawGlyph(char ch, float x, float y, float scale, Color color) {
    // Tiny built-in 3x5 bitmap alphabet for an on-screen controls/status overlay.
    static const std::string glyphs[] = {
        "111101101101111","110101110101110","111100100100111",
        "110101101101110","111100110100111","111100110100100",
        "111100101101111","101101111101101","111010010010111",
        "001001001101111","101101110101101","100100100100111",
        "101111111101101","101111111111101","111101101101111",
        "111101111100100","111101101111011","111101111110101",
        "111100111001111","111010010010010","101101101101111",
        "101101101101010","101101111111101","101101010101101",
        "101101010010010","111001010100111"
    };
    static const std::string digits[] = {
        "111101101101111","010110010010111","111001111100111",
        "111001111001111","101101111001001","111100111001111",
        "111100111101111","111001001001001","111101111101111",
        "111101111001111"
    };
    const std::string* pattern=nullptr;
    if(ch>='A' && ch<='Z') pattern=&glyphs[ch-'A'];
    else if(ch>='0' && ch<='9') pattern=&digits[ch-'0'];
    if(!pattern) return;
    for(int row=0;row<5;++row) for(int col=0;col<3;++col)
        if((*pattern)[row*3+col]=='1')
            rectangle(x+col*scale,y+(4-row)*scale,scale*.85f,scale*.85f,color);
}
void drawText(const std::string& text,float x,float y,float scale,Color color) {
    for(char ch:text) { drawGlyph(ch,x,y,scale,color); x+=scale*4.5f; }
}
struct Vertex3D { float x,y,z,nx,ny,nz; };
std::vector<Vertex3D> makeSunSphere() {
    std::vector<Vertex3D> mesh;
    constexpr int slices=48, stacks=24;
    auto vertex=[](float latitude,float longitude)->Vertex3D {
        float x=std::cos(latitude)*std::cos(longitude);
        float y=std::sin(latitude);
        float z=std::cos(latitude)*std::sin(longitude);
        return {x,y,z,x,y,z};
    };
    for(int row=0;row<stacks;++row) {
        float a=-PI*.5f+PI*row/stacks, b=-PI*.5f+PI*(row+1)/stacks;
        for(int col=0;col<slices;++col) {
            float c=2*PI*col/slices,d=2*PI*(col+1)/slices;
            auto p0=vertex(a,c),p1=vertex(a,d),p2=vertex(b,d),p3=vertex(b,c);
            // Outward-facing winding, compatible with GL_BACK culling.
            mesh.push_back(p0);mesh.push_back(p2);mesh.push_back(p1);
            mesh.push_back(p0);mesh.push_back(p3);mesh.push_back(p2);
        }
    }
    return mesh;
}
#ifdef _WIN32
bool audioCommand(const std::wstring& command) {
    MCIERROR error=mciSendStringW(command.c_str(),nullptr,0,nullptr);
    if(error) {
        wchar_t message[512]={};
        mciGetErrorStringW(error,message,512);
        std::wcerr<<L"Music error: "<<message<<L" | "<<command<<L'\n';
        MessageBoxW(nullptr,(std::wstring(L"Music error: ")+message+L"\nCommand: "+command).c_str(),
                    L"SpiderHero audio error",MB_OK|MB_ICONWARNING);
        return false;
    }
    return true;
}
#endif

GLuint compile(GLenum type, const char* src) {
    GLuint id = glCreateShader(type); glShaderSource(id, 1, &src, nullptr); glCompileShader(id);
    GLint ok = 0; glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
    if (!ok) { char log[2048]; glGetShaderInfoLog(id, 2048, nullptr, log); std::cerr << log << '\n'; }
    return id;
}
int main() {
    if (!glfwInit()) { std::cerr << "GLFW initialization failed\n"; return 1; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "2D Spider Man Animation - 5 Scenes", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(window); glfwSwapInterval(1);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD initialization failed\n"; glfwDestroyWindow(window); glfwTerminate(); return 1;
    }
    std::string shaderDir = "shaders/";
#ifdef _WIN32
    char shaderExe[MAX_PATH] = {};
    GetModuleFileNameA(nullptr,shaderExe,MAX_PATH);
    std::string exeDir(shaderExe);
    size_t lastSlash=exeDir.find_last_of("\\\\/");
    if(lastSlash!=std::string::npos) exeDir.resize(lastSlash+1);
    shaderDir=exeDir+"shaders/";
#endif
    std::string vertexSource=readShader(shaderDir+"scene.vert");
    std::string fragmentSource=readShader(shaderDir+"scene.frag");
    if(vertexSource.empty() || fragmentSource.empty()) {
        std::cerr << "Copy shaders/ next to SpiderHero.exe.\\n";
        glfwDestroyWindow(window); glfwTerminate(); return 1;
    }
    GLuint vs = compile(GL_VERTEX_SHADER, vertexSource.c_str()), fs = compile(GL_FRAGMENT_SHADER, fragmentSource.c_str());
    if (!vs || !fs) { glfwDestroyWindow(window); glfwTerminate(); return 1; }
    GLuint program = glCreateProgram(); glAttachShader(program, vs); glAttachShader(program, fs);
    glLinkProgram(program); glDeleteShader(vs); glDeleteShader(fs);
    GLint linked = 0; glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) { char log[2048]; glGetProgramInfoLog(program, 2048, nullptr, log); std::cerr << log << '\n'; }
    std::string sunVS=readShader(shaderDir+"sun.vert"),sunFS=readShader(shaderDir+"sun.frag");
    if(sunVS.empty()||sunFS.empty()) { glfwDestroyWindow(window);glfwTerminate();return 1; }
    GLuint cv=compile(GL_VERTEX_SHADER,sunVS.c_str()),cf=compile(GL_FRAGMENT_SHADER,sunFS.c_str());
    if(!cv||!cf) { glfwDestroyWindow(window);glfwTerminate();return 1; }
    GLuint sunProgram=glCreateProgram(); glAttachShader(sunProgram,cv);glAttachShader(sunProgram,cf);
    glLinkProgram(sunProgram);glDeleteShader(cv);glDeleteShader(cf);
    GLint sunLinked=0;glGetProgramiv(sunProgram,GL_LINK_STATUS,&sunLinked);
    if(!sunLinked) { char log[2048];glGetProgramInfoLog(sunProgram,2048,nullptr,log);std::cerr<<log<<'\n';return 1; }
    auto sunData=makeSunSphere();
    GLuint sunVAO=0,sunVBO=0;glGenVertexArrays(1,&sunVAO);glGenBuffers(1,&sunVBO);
    glBindVertexArray(sunVAO);glBindBuffer(GL_ARRAY_BUFFER,sunVBO);
    glBufferData(GL_ARRAY_BUFFER,sunData.size()*sizeof(Vertex3D),sunData.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex3D),(void*)0);glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex3D),(void*)(3*sizeof(float)));glEnableVertexAttribArray(1);
    GLuint vao = 0, vbo = 0; glGenVertexArrays(1, &vao); glGenBuffers(1, &vbo);
    glBindVertexArray(vao); glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(float))); glEnableVertexAttribArray(1);
    // MP3 playback through Windows MCI; Space pauses/resumes the audio and animation together.
#ifdef _WIN32
    wchar_t executablePath[MAX_PATH] = {};
    GetModuleFileNameW(nullptr, executablePath, MAX_PATH);
    std::wstring musicPath(executablePath);
    size_t slash = musicPath.find_last_of(L"\\/");
    if (slash != std::wstring::npos) musicPath.resize(slash + 1);
    musicPath += L"song.mp3";
    DWORD attributes=GetFileAttributesW(musicPath.c_str());
    bool musicReady=false;
    if(attributes==INVALID_FILE_ATTRIBUTES || (attributes&FILE_ATTRIBUTE_DIRECTORY)) {
        std::wstring message=L"song.mp3 was not found next to SpiderHero.exe:\n"+musicPath;
        MessageBoxW(nullptr,message.c_str(),L"SpiderHero: missing music",MB_OK|MB_ICONWARNING);
    } else {
        musicReady=audioCommand(L"open \""+musicPath+L"\" type mpegvideo alias bgmusic");
        if(musicReady) musicReady=audioCommand(L"play bgmusic from 0");
    }
#endif
    double start = glfwGetTime(), pauseAt = 0;
    bool paused = false, lastSpace = false, lastR = false;
    bool lighting=true, showHelp=true, show3D=true, showLightMarker=false, lastL=false, lastH=false, lastC=false, last3=false;
    float cameraX=0.f, cameraY=0.f, cameraZ=3.15f;
    float lightX=.0f, lightY=.0f;
    float sunOffsetX=0.f,sunOffsetY=0.f;
    int lastCycle=0;
    double fpsClock=glfwGetTime(); int fpsFrames=0; float fps=0.f;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)glfwSetWindowShouldClose(window, 1);
        bool space = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        if (space && !lastSpace) {
            if (paused) {
                start += glfwGetTime() - pauseAt; paused = false;
#ifdef _WIN32
                if(musicReady) audioCommand(L"resume bgmusic");
#endif
            } else {
                pauseAt = glfwGetTime(); paused = true;
#ifdef _WIN32
                if(musicReady) audioCommand(L"pause bgmusic");
#endif
            }
        }
        lastSpace = space;
        bool r = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;
        if (r && !lastR) {
            start=glfwGetTime();paused=false;lastCycle=0;
#ifdef _WIN32
            if(musicReady) audioCommand(L"play bgmusic from 0");
#endif
        }lastR = r;
        bool l=glfwGetKey(window,GLFW_KEY_L)==GLFW_PRESS;
        if(l && !lastL) lighting=!lighting;
        lastL=l;
        bool h=glfwGetKey(window,GLFW_KEY_H)==GLFW_PRESS || glfwGetKey(window,GLFW_KEY_F1)==GLFW_PRESS;
        if(h && !lastH) showHelp=!showHelp;
        lastH=h;
        bool key3=glfwGetKey(window,GLFW_KEY_3)==GLFW_PRESS;
        if(key3 && !last3) show3D=!show3D;
        last3=key3;
        bool c=glfwGetKey(window,GLFW_KEY_C)==GLFW_PRESS;
        if(c && !lastC) { cameraX=0.f;cameraY=0.f;cameraZ=3.15f;lightX=0.f;lightY=0.f;sunOffsetX=0.f;sunOffsetY=0.f; }
        lastC=c;
        if(glfwGetKey(window,GLFW_KEY_LEFT)==GLFW_PRESS) cameraX=(std::max)(-2.6f,cameraX-.013f);
        if(glfwGetKey(window,GLFW_KEY_RIGHT)==GLFW_PRESS) cameraX=(std::min)(2.6f,cameraX+.013f);
        if(glfwGetKey(window,GLFW_KEY_UP)==GLFW_PRESS) cameraY=(std::min)(.5f,cameraY+.013f);
        if(glfwGetKey(window,GLFW_KEY_DOWN)==GLFW_PRESS) cameraY=(std::max)(-.5f,cameraY-.013f);
        if(glfwGetKey(window,GLFW_KEY_EQUAL)==GLFW_PRESS) cameraZ=(std::max)(1.8f,cameraZ-.016f);
        if(glfwGetKey(window,GLFW_KEY_MINUS)==GLFW_PRESS) cameraZ=(std::min)(5.8f,cameraZ+.016f);
        if(glfwGetKey(window,GLFW_KEY_A)==GLFW_PRESS) sunOffsetX=(std::max)(-2.6f,sunOffsetX-.025f);
        if(glfwGetKey(window,GLFW_KEY_D)==GLFW_PRESS) sunOffsetX=(std::min)(2.6f,sunOffsetX+.025f);
        if(glfwGetKey(window,GLFW_KEY_W)==GLFW_PRESS) sunOffsetY=(std::min)(1.5f,sunOffsetY+.025f);
        if(glfwGetKey(window,GLFW_KEY_S)==GLFW_PRESS) sunOffsetY=(std::max)(-1.5f,sunOffsetY-.025f);
        float elapsed=(float)((paused ? pauseAt : glfwGetTime())-start);
        int cycle=(int)(elapsed/108.f);
        if(cycle!=lastCycle) {
            lastCycle=cycle;
#ifdef _WIN32
            if(musicReady) audioCommand(L"play bgmusic from 0");
#endif
        }
        float t=std::fmod(elapsed,108.f);
        float baseSunX=.71f,baseSunY=.59f;
        if(t>=20.f && t<55.f) {baseSunX=.76f;baseSunY=.69f;}
        else if(t>=67.f && t<74.f) {baseSunX=.70f;baseSunY=.67f;}
        else if(t>=74.f) {baseSunX=.60f;baseSunY=.18f-.34f*phase(t-74.f,0.f,24.f);}
        lightX=baseSunX+sunOffsetX;
        lightY=baseSunY+sunOffsetY;
        vertices.clear();
        // Extended, repeatable city beyond the camera frame. These are background-only tiles.
        rectangle(-12.f,-4.f,24.f,8.f, t<55.f ? Color{.54f,.66f,.83f} : Color{.17f,.24f,.42f});
        for(int tile=-3;tile<=3;++tile) {
            if(tile==0) continue;
            size_t begin=vertices.size();
            if(t>=20.f && t<55.f) street();
            else sky();
            for(size_t i=begin;i<vertices.size();++i) vertices[i].x+=tile*2.f;
        }
        if (t < 20.f)sceneOne(t);
        else if (t < 55.f)sceneTwo(t - 20.f);
        else if (t < 67.f)comedyScene(t - 55.f);
        else if (t < 74.f)chaseCity(t - 67.f);
        else sceneFour(t - 74.f);
        if(showLightMarker) {
            ellipse(lightX,lightY,.034f,.034f,{1.f,.98f,.62f},24);
            line(lightX-.055f,lightY,lightX+.055f,lightY,.006f,white);
            line(lightX,lightY-.055f,lightX,lightY+.055f,.006f,white);
        }
        size_t sceneVertexCount=vertices.size();
        // Overlay is deliberately drawn unlit, preserving the original comic palette.
        if(showHelp) {
            rectangle(-1.f,.80f,2.f,.20f,{.065f,.085f,.15f});
            drawText("SPACE PAUSE   R RESTART   L LIGHT   H HIDE",-.95f,.927f,.011f,white);
            drawText("WASD MOVE SUN   ARROWS MOVE CAMERA",-.95f,.869f,.011f,white);
            drawText("C RESET   3 SHOW SUN   PLUS MINUS ZOOM",-.95f,.812f,.010f,white);
        }
        size_t overlayVertexCount=vertices.size()-sceneVertexCount;
        int width, height; glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClearColor(.95f, .72f, .8f, 1); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program); glBindVertexArray(vao); glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
        float aspect=height>0 ? float(width)/height : 16.f/9.f;
        Mat4 proj=perspective(2.f*std::atan(1.f/3.15f),aspect,.1f,20.f);
        Mat4 view=cameraView(cameraX,cameraY,cameraZ);
        glUniformMatrix4fv(glGetUniformLocation(program,"uProjection"),1,GL_FALSE,proj.v);
        glUniformMatrix4fv(glGetUniformLocation(program,"uView"),1,GL_FALSE,view.v);
        glUniform1f(glGetUniformLocation(program,"uAspect"),aspect);
        glUniform2f(glGetUniformLocation(program,"uLightPos"),lightX,lightY);
        glUniform1i(glGetUniformLocation(program,"uLighting"),lighting?1:0);
        glUniform1i(glGetUniformLocation(program,"uOverlay"),0);
        // Painter-order 2D layers overlap intentionally, so depth/culling are disabled
        // for the 2D animation pass. They must be enabled for actual 3D meshes.
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDrawArrays(GL_TRIANGLES,0,(GLsizei)sceneVertexCount);
        if(show3D && t<103.f) {
            // Real 3D sun sphere with surface normals, perspective and depth testing.
            glEnable(GL_DEPTH_TEST);glEnable(GL_CULL_FACE);glCullFace(GL_BACK);
            glUseProgram(sunProgram);glBindVertexArray(sunVAO);
            glUniformMatrix4fv(glGetUniformLocation(sunProgram,"uProjection"),1,GL_FALSE,proj.v);
            glUniformMatrix4fv(glGetUniformLocation(sunProgram,"uView"),1,GL_FALSE,view.v);
            glUniform1f(glGetUniformLocation(sunProgram,"uTime"),t);
            glUniform1f(glGetUniformLocation(sunProgram,"uAspect"),aspect);
            glUniform2f(glGetUniformLocation(sunProgram,"uSunPos"),lightX,lightY);
            glUniform2f(glGetUniformLocation(sunProgram,"uLightPos"),lightX,lightY);
            glUniform1i(glGetUniformLocation(sunProgram,"uLighting"),lighting?1:0);
            glDrawArrays(GL_TRIANGLES,0,(GLsizei)sunData.size());
            glDisable(GL_CULL_FACE);glDisable(GL_DEPTH_TEST);
            glUseProgram(program);glBindVertexArray(vao);
        }
        if(overlayVertexCount) {
            glUniform1i(glGetUniformLocation(program,"uOverlay"),1);
            glDrawArrays(GL_TRIANGLES,(GLint)sceneVertexCount,(GLsizei)overlayVertexCount);
        }
        fpsFrames++;
        double now=glfwGetTime();
        if(now-fpsClock>=1.0) {
            fps=float(fpsFrames/(now-fpsClock));fpsClock=now;fpsFrames=0;
            std::ostringstream title;
            title<<"2D Spider Man Animation | "<<int(fps)<<" FPS | Light "
                 <<(lighting?"ON":"OFF")<<" | H help";
            glfwSetWindowTitle(window,title.str().c_str());
        }
        glfwSwapBuffers(window);
    }
#ifdef _WIN32
    if(musicReady) audioCommand(L"close bgmusic");
#endif
    glDeleteBuffers(1, &vbo); glDeleteVertexArrays(1, &vao); glDeleteProgram(program);
    glDeleteBuffers(1,&sunVBO);glDeleteVertexArrays(1,&sunVAO);glDeleteProgram(sunProgram);
    glfwDestroyWindow(window); glfwTerminate(); return 0;
}
