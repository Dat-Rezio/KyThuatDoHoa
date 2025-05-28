#include <iostream>
#include <bits/stdc++.h>
#include <GL/glut.h>
using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

float angleX = 0.0f, angleY = 0.0f; // Góc xoay quanh trục X, Y
float zoom = 1.0f;                  // Hệ số zoom
int lastX, lastY;                   // Vị trí chuột lần trước
bool leftButtonDown = false;
bool rightButtonDown = false;

struct Point3D {
    float x, y, z, w;
    Point3D(float _x=0, float _y=0, float _z=0, float _w=1.0f)
        : x(_x), y(_y), z(_z), w(_w) {}
};

struct Matrix4x4 {
    float m[4][4];

    static Matrix4x4 identity() {
        Matrix4x4 mat;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                mat.m[i][j] = (i == j) ? 1.0f : 0.0f;
        return mat;
    }

    static Matrix4x4 scale(float sx, float sy, float sz) {
        Matrix4x4 mat = identity();
        mat.m[0][0] = sx;
        mat.m[1][1] = sy;
        mat.m[2][2] = sz;
        return mat;
    }
	
    static Matrix4x4 translate(float dx, float dy, float dz) {
        Matrix4x4 mat = identity();
        mat.m[3][0] = dx;
        mat.m[3][1] = dy;
        mat.m[3][2] = dz;
        return mat;
    }

    static Matrix4x4 shear(float a, float b, float c, float d, float e, float f) {
        Matrix4x4 mat = identity();
        mat.m[1][0] = a;
        mat.m[2][0] = b;
        mat.m[0][1] = c;
        mat.m[2][1] = d;
        mat.m[0][2] = e;
        mat.m[1][2] = f;
        return mat;
    }
	
    Point3D apply(const Point3D &v) const {
        Point3D r;
        r.x = m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w;
        r.y = m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w;
        r.z = m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * v.w;
        r.w = m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3] * v.w;
        return r;
    }
};

Point3D A(0, 0, 0), B(1, 0, 0), C(0, 1, 0), D(0, 0, 1);
Point3D A2, B2, C2, D2;

void drawTetrahedron(Point3D a, Point3D b, Point3D c, Point3D d, bool wireframe = true) {
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    glBegin(GL_TRIANGLES);
    glVertex3f(a.x, a.y, a.z); glVertex3f(b.x, b.y, b.z); glVertex3f(c.x, c.y, c.z);
    glVertex3f(a.x, a.y, a.z); glVertex3f(b.x, b.y, b.z); glVertex3f(d.x, d.y, d.z);
    glVertex3f(a.x, a.y, a.z); glVertex3f(c.x, c.y, c.z); glVertex3f(d.x, d.y, d.z);
    glVertex3f(b.x, b.y, b.z); glVertex3f(c.x, c.y, c.z); glVertex3f(d.x, d.y, d.z);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    gluLookAt(0, 0, 5 / zoom, 0, 0, 0, 0, 1, 0); // zoom ảnh hưởng khoảng cách camera
    glRotatef(angleY, 1.0f, 0.0f, 0.0f); // xoay quanh X
    glRotatef(angleX, 0.0f, 1.0f, 0.0f); // xoay quanh Y

    glColor3f(1, 1, 1);
    drawTetrahedron(A, B, C, D); 

    glColor3f(0, 1, 0);
    drawTetrahedron(A2, B2, C2, D2);

    glutSwapBuffers();
}
void reshape(int w, int h) {
    if (h == 0) h = 1; // tránh chia cho 0
    float aspect = (float)w / (float)h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspect, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}
void initOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0, 1.0, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON)
        leftButtonDown = (state == GLUT_DOWN);
    else if (button == GLUT_RIGHT_BUTTON)
        rightButtonDown = (state == GLUT_DOWN);

    lastX = x;
    lastY = y;
}

void motion(int x, int y) {
    int dx = x - lastX;
    int dy = y - lastY;

    if (leftButtonDown) {
        angleX += dx * 0.5f;
        angleY += dy * 0.5f;
    }

    if (rightButtonDown) {
        zoom += dy * 0.01f;
        if (zoom < 0.1f) zoom = 0.1f;
        if (zoom > 10.0f) zoom = 10.0f;
    }

    lastX = x;
    lastY = y;

    glutPostRedisplay(); // yêu cầu vẽ lại
}

int main(int argc, char** argv) {
    int choice;
    cout << "Chon phep bien doi:\n";
    cout << "1. Scale\n2. Shear\n3. Translate\n";
    cout << "Nhap lua chon: ";
    cin >> choice;

    Matrix4x4 transform = Matrix4x4::identity();
    switch (choice) {
        case 1:
            cout << "Nhap he so scale (sx, sy, sz): ";
            float sx, sy, sz;
            cin >> sx >> sy >> sz;
            transform = Matrix4x4::scale(sx, sy, sz);
            break;
        case 2:
            cout << "Nhap he so shear (a, b, c, d, e, f): ";
            float a, b, c, d, e, f;
            cin >> a >> b >> c >> d >> e >> f;
            transform = Matrix4x4::shear(a, b, c, d, e, f);
            break;
        case 3:
            cout << "Nhap he so dich chuyen (dx, dy, dz): ";
            float dx, dy, dz;
            cin >> dx >> dy >> dz;
            transform = Matrix4x4::translate(dx, dy, dz);
            break;
        default:
            cout << "Lua chon khong hop le! Su dung phep bien doi mac dinh.\n";
            transform = Matrix4x4::identity();
            break;
    }

    A2 = transform.apply(A);
    B2 = transform.apply(B);
    C2 = transform.apply(C);
    D2 = transform.apply(D);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Tetrahedron Transform");

    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    initOpenGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMainLoop();
    return 0;
}


