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
bool middleButtonDown = false;

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
Point3D center;
Point3D A2, B2, C2, D2;

Point3D computeCentroid(Point3D a, Point3D b, Point3D c, Point3D d) {
    return Point3D(
        (a.x + b.x + c.x + d.x) / 4.0f,
        (a.y + b.y + c.y + d.y) / 4.0f,
        (a.z + b.z + c.z + d.z) / 4.0f
    );
}

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
    
    gluLookAt(0, 0, 5 / zoom, center.x, center.y, center.z, 0, 1, 0); // zoom ảnh hưởng khoảng cách camera, nhìn vào tâm hình
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
    else if (button == GLUT_MIDDLE_BUTTON)
        middleButtonDown = (state == GLUT_DOWN);

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

    if (middleButtonDown) {
        float panSpeed = 0.01f * zoom; // tốc độ pan tỉ lệ với zoom

        // Tính toán vector pan trong không gian camera
        center.x -= dx * panSpeed;
        center.y += dy * panSpeed; // trục y ngược vì tọa độ màn hình y đi xuống
    }

    lastX = x;
    lastY = y;

    glutPostRedisplay(); // yêu cầu vẽ lại
}

int main(int argc, char** argv) {
    int choice;
    cout << "Nhập tọa độ các đỉnh của tứ diện:\n";
    cout << "A (x, y, z): ";
    cin >> A.x >> A.y >> A.z;
    cout << "B (x, y, z): ";
    cin >> B.x >> B.y >> B.z;
    cout << "C (x, y, z): ";
    cin >> C.x >> C.y >> C.z;
    cout << "D (x, y, z): ";
    cin >> D.x >> D.y >> D.z;
    center = computeCentroid(A, B, C, D);
    A2 = A; B2 = B; C2 = C; D2 = D; // Sao chép tọa độ ban đầu
    
    
    
    bool control = true;
    while(control) {
        cout << "Chọn phép biến đổi:\n";
        cout << "1. Scale\n2. Shear\n3. Translate\n";
        cout << "Nhập lựa chọn: ";
        cin >> choice;

        Matrix4x4 transform = Matrix4x4::identity();
        switch (choice) {
            case 1:
                cout << "Ma trận Scale:\n";
                cout << "sx 0  0  0\n";
                cout << "0  sy 0  0\n";
                cout << "0  0  sz 0\n";
                cout << "0  0  0  1\n";
                cout << "Nhập hệ số (sx, sy, sz): ";
                float sx, sy, sz;
                cin >> sx >> sy >> sz;
                transform = Matrix4x4::scale(sx, sy, sz);
                control = false;
                break;
            case 2:
                cout << "Ma trận Shear:\n";
                cout << "1 c e 0\n";
                cout << "a 1 f 0\n";
                cout << "b d 1 0\n";
                cout << "0 0 0 1\n";
                cout << "Nhập hệ số (a, b, c, d, e, f):";
                float a, b, c, d, e, f;
                cin >> a >> b >> c >> d >> e >> f;
                transform = Matrix4x4::shear(a, b, c, d, e, f);
                control = false;
                break;
            case 3:
                cout << "Ma trận Translate:\n";
                cout << "1  0  0  0\n";
                cout << "0  1  0  0\n";
                cout << "0  0  1  0\n";
                cout << "dx dy dz 1\n";
                cout << "Nhập hệ số (dx, dy, dz): ";
                float dx, dy, dz;
                cin >> dx >> dy >> dz;
                transform = Matrix4x4::translate(dx, dy, dz);
                control = false;
                break;
            default:
                cout << "Lựa chọn không hợp lệ! Vui lòng chọn lại!\n";
                transform = Matrix4x4::identity();
                break;
        }
        if (control == false) {
            A2 = transform.apply(A);
            B2 = transform.apply(B);
            C2 = transform.apply(C);
            D2 = transform.apply(D);
            cout << "Dieu khien: \n";
            cout << "1. Xoay (dùng chuột trái)\n";
            cout << "2. Zoom (dùng chuột phải)\n";
            cout << "3. Di chuyển (dùng chuột giữa)\n";
        }
    }
    
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


