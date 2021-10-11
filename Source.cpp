#include <iostream>
#include <math.h>
#include <GL/glut.h>
#include <time.h>
#include "windows.h"
#include "mmsystem.h"
#pragma comment(lib,"winmm.lib")

using namespace std;

/*重製計數陣列*/
void ZeroTimes(int**);
void mainMenu(int);
/*用來繪出所有長形物體*/
void cylinder();
/*上臂*/
void arm_1();
/*手肘*/
void joint_1();
/*下臂*/
void arm_2();
/*手腕*/
void joint_2();
/*大拇指*/
void thumb(double[3]);
/*食指*/
void forefinger(double[3]);
/*中指*/
void middlefinger(double[3]);
/*無名指*/
void ringfinger(double[3]);
/*小拇指*/
void littlefinger(double[3]);
/*電腦手臂*/
void hand_1();
/*玩家手臂*/
void hand_2();
/*出剪刀*/
bool Scissor(double[3], double[3], double[3], double[3], double[3], int**);
/*出石頭*/
bool Rock(double[3], double[3], double[3], double[3], double[3], int**);
/*出布*/
bool Papper(double[3], double[3], double[3], double[3], double[3], int**);
/*將下臂往上*/
void handShakeUp();
/*將下臂往下*/
void handShakeDown();
/*輸出猜拳輸贏結果*/
int result(int, int);
void display();
void myInit();
/*用按鍵操作電腦手臂*/
void mykey(unsigned char key, int x, int y);
/*用滑鼠操控視角*/
void mymotion(int x, int y);
/*設定光源屬性*/
GLfloat light0_position[] = { 5.0, 5.0, 5.0, 1.0 };
GLfloat light0_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat light0_diffuse[] = { 0.2, 0.2, 0.2, 1.0 };
GLfloat light0_specular[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat light1_position[] = { 40.0, 5.0, 5.0, 1.0 };
GLfloat light1_ambient[] = { 0.1, 0.0, 0.0, 1.0 };
GLfloat light1_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light1_specular[] = { 0.0, 0.0, 0.0, 1.0 };
/*紀錄前一個x, y值*/
GLfloat nowX = 0,
nowY = 0;
/*紀錄r, phi以及theta的值*/
GLfloat r = 1,
phi = 0,
theta = 0;
/*最終放置至gluLookAt的x, y, z值*/
GLfloat lookAtX = 1, lookAtY = 1, lookAtZ = 0;
/*手指各關節*/
double thumb_joint_1[3] = { 0 },
forefinger_joint_1[3] = { 0 },
middlefinger_joint_1[3] = { 0 },
ringfinger_joint_1[3] = { 0 },
littlefinger_joint_1[3] = { 0 },
thumb_joint_2[3] = { 0 },
forefinger_joint_2[3] = { 0 },
middlefinger_joint_2[3] = { 0 },
ringfinger_joint_2[3] = { 0 },
littlefinger_joint_2[3] = { 0 };
/*剪刀石頭布的關節位置*/
float rock[3][5] = { {-48, 0, 0, 0, 0},
					{32, -96, -96, -96, -90},
					{56, -92, -114, -114, -72} },
	scissor[3][5] = { {-50, -10, 0, 0, 0},
					{40, 4, 8, -90, -100},
					{80, -9, -16, -70, -50} },
	papper[3][5] = { {-8.5, 0, 0, 0, 8},
						{-8.5, 4, 8, 0, 0},
						{-1, -9, -26, 0, -4} };
/*欲操控之手指1為大拇指，2為食指...以此類推*/
int finger = 1;
/*手肘旋轉角度*/
float elbow = 9;
/*紀錄抵達關節位置次數之陣列*/
int** times_1, **times_2;
/*紀錄電腦及玩家之出拳種類*/
int type_1 = 0,
type_2 = 0,
/*紀錄出拳前之甩手*/
shakeTimes;
/*手指旋轉速度*/
float add = 7;
bool finish[2] = {false}, type, start = false, stop = false;

int main(int argc, char** argv) {
	times_1 = new int* [3];
	times_2 = new int* [3];
	for (int i = 0; i < 3; i++) {
		times_1[i] = new int[5];
		times_2[i] = new int[5];
	}
	srand(time(NULL));
	ZeroTimes(times_1);
	ZeroTimes(times_2);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("3D Robot");
	glutDisplayFunc(display);
	glutKeyboardFunc(mykey);
	glutPassiveMotionFunc(mymotion);
	int menu = glutCreateMenu(mainMenu);
	glutAddMenuEntry("剪刀", 1);
	glutAddMenuEntry("石頭", 2);
	glutAddMenuEntry("布", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	myInit();
	glutMainLoop();
}

void ZeroTimes(int** times) {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 5; j++) {
			times[i][j] = 0;
		}
	}
}

void mainMenu(int index) {
	shakeTimes = 4;
	type = false;
	type_1 = (rand() % 3) + 1;
	type_2 = index;
	finish[0] = false;
	finish[1] = false;
	start = true;
	stop = false;
	glutPostRedisplay();
}

void cylinder() {
	GLUquadricObj* objCylinder = gluNewQuadric();
	gluCylinder(objCylinder, 1, 1, 1, 100, 5);
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	gluLookAt(lookAtX, lookAtY, lookAtZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glPushMatrix();
	glTranslatef(50, 0, 0);
	glRotatef(90, -1.0, -1.0, 0.0);
	glRotatef(-135, 0.0, 0.0, 1.0);
	hand_1();
	glPopMatrix();
	glPushMatrix();

	glTranslatef(-50, 0, 0);
	glRotatef(90, 1.0, -1.0, 0.0);
	glRotatef(135, 0.0, 0.0, 1.0);
	glRotatef(180, 0.0, 1.0, 0.0);
	hand_2();
	glPopMatrix();
	
	if (shakeTimes) {
		/*猜拳前之甩手*/
		if (shakeTimes == 4) {
			/*先收手*/
			finish[0] = Rock(thumb_joint_1, forefinger_joint_1, middlefinger_joint_1, ringfinger_joint_1, littlefinger_joint_1, times_1);
			finish[1] = Rock(thumb_joint_2, forefinger_joint_2, middlefinger_joint_2, ringfinger_joint_2, littlefinger_joint_2, times_2);
			if (finish[0] && finish[1]) {
				shakeTimes -= 1;
				finish[0] = false;
				finish[1] = false;
				ZeroTimes(times_1);
				ZeroTimes(times_2);
			}
			glutPostRedisplay();
		}
		/*開始甩手*/
		else {
			if (start) {
				PlaySound(TEXT("pss2.wav"), NULL, SND_FILENAME | SND_ASYNC);
				start = false;
			}
			if (type) {
				handShakeUp();
			}
			else {
				handShakeDown();
			}
		}
	}
	else {
		/*出拳結束後的結果輸出*/
		if (finish[0] && finish[1] && !stop) {
			Sleep(300);
			if (result(type_2, type_1) == 1) {
				cout << "恭喜，你贏了！" << endl;
				PlaySound(TEXT("win.wav"), NULL, SND_FILENAME | SND_SYNC);
			}
			else if (result(type_2, type_1) == -1) {
				cout << "真可惜，你輸了！" << endl;
				PlaySound(TEXT("loss.wav"), NULL, SND_FILENAME | SND_SYNC);
			}
			else {
				cout << "平手" << endl;
				PlaySound(TEXT("again.wav"), NULL, SND_FILENAME | SND_SYNC);
			}
			ZeroTimes(times_1);
			ZeroTimes(times_2);
			stop = true;
		}
		else {
			/*玩家出拳*/
			switch (type_1) {
			case 1:
				finish[0] = Scissor(thumb_joint_1, forefinger_joint_1, middlefinger_joint_1, ringfinger_joint_1, littlefinger_joint_1, times_1);
				glutPostRedisplay();
				break;
			case 2:
				finish[0] = Rock(thumb_joint_1, forefinger_joint_1, middlefinger_joint_1, ringfinger_joint_1, littlefinger_joint_1, times_1);
				glutPostRedisplay();
				break;
			case 3:
				finish[0] = Papper(thumb_joint_1, forefinger_joint_1, middlefinger_joint_1, ringfinger_joint_1, littlefinger_joint_1, times_1);
				glutPostRedisplay();
				break;
			default:
				break;
			}
			/*電腦出拳*/
			switch (type_2) {
			case 1:
				finish[1] = Scissor(thumb_joint_2, forefinger_joint_2, middlefinger_joint_2, ringfinger_joint_2, littlefinger_joint_2, times_2);
				glutPostRedisplay();
				break;
			case 2:
				finish[1] = Rock(thumb_joint_2, forefinger_joint_2, middlefinger_joint_2, ringfinger_joint_2, littlefinger_joint_2, times_2);
				glutPostRedisplay();
				break;
			case 3:
				finish[1] = Papper(thumb_joint_2, forefinger_joint_2, middlefinger_joint_2, ringfinger_joint_2, littlefinger_joint_2, times_2);
				glutPostRedisplay();
				break;
			default:
				break;
			}
		}
	}
	glFlush();
}

void arm_1() {
	glColor3f(0.988, 0.901, 0.788);
	glPushMatrix();
	glRotatef(90, 0.0, 1.0, 0.0);
	glScaled(4, 4, 20);
	cylinder();
	glPopMatrix();
}

void arm_2() {
	glColor3f(0.988, 0.901, 0.788);
	glPushMatrix();
	glScalef(3.7, 3.7, 18);
	cylinder();
	glPopMatrix();
}

void joint_1() {
	glColor3f(0.988, 0.901, 0.788);
	glPushMatrix();
	glScalef(5, 5, 5);
	glutSolidSphere(1, 1000, 1000);
	glPopMatrix();
}

void joint_2() {
	glColor3f(0.988, 0.901, 0.788);
	glPushMatrix();
	glScalef(4, 4, 4);
	glutSolidSphere(1, 1000, 1000);
	glPopMatrix();
}

void thumb(double thumb_joint[3]) {
	glColor3f(0.988, 0.901, 0.788);
	glPushMatrix();
	glRotatef(-48, 0.0, 1.0, 0.0);
	glRotatef(thumb_joint[0], 1.0, 0.0, 0.0);
	glRotatef(thumb_joint[1], 0.0, 1.0, 0.0);
	glTranslatef(-1.8, 0, 3);
	glPushMatrix();
	glScaled(0.9, 0.9, 3);
	cylinder();
	glPopMatrix();

	glTranslatef(0, 0, 3.3);
	glColor3f(0.988, 0.901, 0.788);
	glPushMatrix();
	glScalef(1.1, 1.1, 1.1);
	glutSolidSphere(1, 1000, 1000);
	glPopMatrix();

	glColor3f(0.988, 0.901, 0.788);
	glRotatef(thumb_joint[2], 0.0, 1.0, 0.0);
	glPushMatrix();
	glScaled(0.9, 0.9, 2.8);
	cylinder();
	glPopMatrix();
	glPopMatrix();
}

void forefinger(double forefinger_joint[3]) {
	glColor3f(0.988, 0.901, 0.788);
	glPushMatrix();
	glTranslatef(-2, 0, 0);
	glRotatef(forefinger_joint[0], 0.0, 1.0, 0.0);
	glTranslatef(0, 0, 2.5);
	glPushMatrix();
	glScaled(0.8, 0.8, 3);
	cylinder();
	glPopMatrix();

	glTranslatef(0, 0, 2.8);
	glColor3f(0.988, 0.901, 0.788);
	glPushMatrix();
	glScalef(0.9, 0.9, 0.9);
	glutSolidSphere(1, 1000, 1000);
	glPopMatrix();

	glColor3f(0.988, 0.901, 0.788);
	glRotatef(forefinger_joint[1], 1.0, 0.0, 0.0);
	glPushMatrix();
	glScaled(0.8, 0.8, 2.8);
	cylinder();
	glPopMatrix();

	glTranslatef(0, 0, 2.6);
	glColor3f(0.988, 0.901, 0.788);
	glPushMatrix();
	glScalef(0.9, 0.9, 0.9);
	glutSolidSphere(1, 1000, 1000);
	glPopMatrix();

	glColor3f(0.988, 0.901, 0.788);
	glRotatef(forefinger_joint[2], 1.0, 0.0, 0.0);
	glPushMatrix();
	glScaled(0.8, 0.8, 2.3);
	cylinder();
	glPopMatrix();

	glPopMatrix();
}

void middlefinger(double middlefinger_joint[3]) {
	glColor3f(0.988, 0.901, 0.788);
	glPushMatrix();
	glTranslatef(0, 0, 0);
	glRotatef(middlefinger_joint[0], 0.0, 1.0, 0.0);
	glTranslatef(0, 0, 3.4);
	glPushMatrix();
	glScaled(0.8, 0.8, 3);
	cylinder();
	glPopMatrix();

	glTranslatef(0, 0, 2.8);
	glColor3f(0.988, 0.901, 0.788);
	glPushMatrix();
	glScalef(0.9, 0.9, 0.9);
	glutSolidSphere(1, 1000, 1000);
	glPopMatrix();

	glColor3f(0.988, 0.901, 0.788);
	glRotatef(middlefinger_joint[1], 1.0, 0.0, 0.0);
	glPushMatrix();
	glScaled(0.8, 0.8, 2.8);
	cylinder();
	glPopMatrix();

	glTranslatef(0, 0, 2.6);
	glColor3f(0.988, 0.901, 0.788);
	glPushMatrix();
	glScalef(0.9, 0.9, 0.9);
	glutSolidSphere(1, 1000, 1000);
	glPopMatrix();

	glColor3f(0.988, 0.901, 0.788);
	glRotatef(middlefinger_joint[2], 1.0, 0.0, 0.0);
	glPushMatrix();
	glScaled(0.8, 0.8, 2.3);
	cylinder();
	glPopMatrix();

	glPopMatrix();
}

void ringfinger(double ringfinger_joint[3]) {
	glColor3f(0.988, 0.901, 0.788);
	glPushMatrix();
	glTranslatef(2, 0, 0);
	glRotatef(ringfinger_joint[0], 0.0, 1.0, 0.0);
	glTranslatef(0, 0, 3.2);
	glPushMatrix();
	glScaled(0.8, 0.8, 3);
	cylinder();
	glPopMatrix();

	glTranslatef(0, 0, 2.8);
	glColor3f(0.988, 0.901, 0.788);
	glPushMatrix();
	glScalef(0.9, 0.9, 0.9);
	glutSolidSphere(1, 1000, 1000);
	glPopMatrix();

	glColor3f(0.988, 0.901, 0.788);
	glRotatef(ringfinger_joint[1], 1.0, 0.0, 0.0);
	glPushMatrix();
	glScaled(0.8, 0.8, 2.8);
	cylinder();
	glPopMatrix();

	glTranslatef(0, 0, 2.6);
	glColor3f(0.988, 0.901, 0.788);
	glPushMatrix();
	glScalef(0.9, 0.9, 0.9);
	glutSolidSphere(1, 1000, 1000);
	glPopMatrix();

	glColor3f(0.988, 0.901, 0.788);
	glRotatef(ringfinger_joint[2], 1.0, 0.0, 0.0);
	glPushMatrix();
	glScaled(0.8, 0.8, 2.3);
	cylinder();
	glPopMatrix();

	glPopMatrix();
}

void littlefinger(double littlefinger_joint[3]) {
	glColor3f(0.988, 0.901, 0.788);
	glPushMatrix();
	glTranslatef(3, 0, 0);
	glRotatef(littlefinger_joint[0], 0.0, 1.0, 0.0);
	glTranslatef(0, 0, 2);
	glPushMatrix();
	glScaled(0.8, 0.8, 2.6);
	cylinder();
	glPopMatrix();

	glTranslatef(0, 0, 2.8);
	glColor3f(0.988, 0.901, 0.788);
	glPushMatrix();
	glScalef(0.9, 0.9, 0.9);
	glutSolidSphere(1, 1000, 1000);
	glPopMatrix();

	glColor3f(0.988, 0.901, 0.788);
	glRotatef(littlefinger_joint[1], 1.0, 0.0, 0.0);
	glPushMatrix();
	glScaled(0.8, 0.8, 2.6);
	cylinder();
	glPopMatrix();

	glTranslatef(0, 0, 2.6);
	glColor3f(0.988, 0.901, 0.788);
	glPushMatrix();
	glScalef(0.9, 0.9, 0.9);
	glutSolidSphere(1, 1000, 1000);
	glPopMatrix();

	glColor3f(0.988, 0.901, 0.788);
	glRotatef(littlefinger_joint[2], 1.0, 0.0, 0.0);
	glPushMatrix();
	glScaled(0.8, 0.8, 2);
	cylinder();
	glPopMatrix();

	glPopMatrix();
}

void hand_1() {
	arm_1();
	glTranslated(17.5, 0.0, 0.0);
	glRotatef(elbow, 0.0, 1.0, 0.0);
	joint_1();
	arm_2();
	glTranslated(0.0, 0.0, 18.0);
	joint_2();
	thumb(thumb_joint_1);
	forefinger(forefinger_joint_1);
	ringfinger(ringfinger_joint_1);
	littlefinger(littlefinger_joint_1);
	middlefinger(middlefinger_joint_1);
}

void hand_2() {
	arm_1();
	glTranslated(17.5, 0.0, 0.0);
	glRotatef(elbow, 0.0, 1.0, 0.0);
	joint_1();
	arm_2();
	glTranslated(0.0, 0.0, 18.0);
	joint_2();
	thumb(thumb_joint_2);
	forefinger(forefinger_joint_2);
	ringfinger(ringfinger_joint_2);
	littlefinger(littlefinger_joint_2);
	middlefinger(middlefinger_joint_2);
}

bool Scissor(double thumb_joint[3], double forefinger_joint[3], double middlefinger_joint[3], double ringfinger_joint[3], double littlefinger_joint[3], int** times) {
	bool finish = true;
	for (int i = 0; i < 3; i++) {
		if (abs(thumb_joint[i] - scissor[i][0]) < add * 0.5) {
			thumb_joint[i] = scissor[i][0];
			times[i][0] += 1;
		}
		if (times[i][0]) {
			continue;
		}
		else {
			finish = false;
		}
		if (thumb_joint[i] > scissor[i][0]) {
			thumb_joint[i] -= add * 0.5;
		}
		else if (thumb_joint[i] < scissor[i][0]) {
			thumb_joint[i] += add * 0.5;
		}
	}
	for (int i = 0; i < 3; i++) {
		if (abs(forefinger_joint[i] - scissor[i][1]) < add * 1) {
			forefinger_joint[i] = scissor[i][1];
			times[i][1] += 1;
		}
		if (times[i][1]) {
			continue;
		}
		else {
			finish = false;
		}
		if (forefinger_joint[i] > scissor[i][1]) {
			forefinger_joint[i] -= add * 1;
		}
		else if (forefinger_joint[i] < scissor[i][1]) {
			forefinger_joint[i] += add * 1;
		}
	}
	for (int i = 0; i < 3; i++) {
		if (abs(middlefinger_joint[i] - scissor[i][2]) < add * 1) {
			middlefinger_joint[i] = scissor[i][2];
			times[i][2] += 1;
		}
		if (times[i][2]) {
			continue;
		}
		else {
			finish = false;
		}
		if (middlefinger_joint[i] > scissor[i][2]) {
			middlefinger_joint[i] -= add * 1;
		}
		else if (middlefinger_joint[i] < scissor[i][2]) {
			middlefinger_joint[i] += add * 1;
		}
	}
	for (int i = 0; i < 3; i++) {
		if (abs(ringfinger_joint[i] - scissor[i][3]) < add * 1) {
			ringfinger_joint[i] = scissor[i][3];
			times[i][3] += 1;
		}
		if (times[i][3]) {
			continue;
		}
		else {
			finish = false;
		}
		if (ringfinger_joint[i] > scissor[i][3]) {
			ringfinger_joint[i] -= add * 1;
		}
		else if (ringfinger_joint[i] < scissor[i][3]) {
			ringfinger_joint[i] += add * 1;
		}
	}
	for (int i = 0; i < 3; i++) {
		if (abs(littlefinger_joint[i] - scissor[i][4]) < add * 1) {
			littlefinger_joint[i] = scissor[i][4];
			times[i][4] += 1;
		}
		if (times[i][4]) {
			continue;
		}
		else {
			finish = false;
		}
		if (littlefinger_joint[i] > scissor[i][4]) {
			littlefinger_joint[i] -= add * 1.3;
		}
		else if (littlefinger_joint[i] < scissor[i][4]) {
			littlefinger_joint[i] += add * 1.3;
		}
	}
	return finish;
}

bool Rock(double thumb_joint[3], double forefinger_joint[3], double middlefinger_joint[3], double ringfinger_joint[3], double littlefinger_joint[3], int** times) {
	bool finish = true;
	for (int i = 0; i < 3; i++) {
		if (abs(thumb_joint[i] - rock[i][0]) < add * 0.5) {
			times[i][0] += 1;
		}
		if (times[i][0]) {
			continue;
		}
		else {
			finish = false;
		}
		if (thumb_joint[i] > rock[i][0]) {
			thumb_joint[i] -= add * 0.5;
		}
		else if (thumb_joint[i] < rock[i][0]) {
			thumb_joint[i] += add * 0.5;
		}
	}
	for (int i = 0; i < 3; i++) {
		if (abs(forefinger_joint[i] - rock[i][1]) < add * 1.2) {
			times[i][1] += 1;
		}
		if (times[i][1]) {
			continue;
		}
		else {
			finish = false;
		}
		if (forefinger_joint[i] > rock[i][1]) {
			forefinger_joint[i] -= add * 1.2;
		}
		else if (forefinger_joint[i] < rock[i][1]) {
			forefinger_joint[i] += add * 1.2;
		}
	}
	for (int i = 0; i < 3; i++) {
		if (abs(middlefinger_joint[i] - rock[i][2]) < add * 1.2) {
			times[i][2] += 1;
		}
		if (times[i][2]) {
			continue;
		}
		else {
			finish = false;
		}
		if (middlefinger_joint[i] > rock[i][2]) {
			middlefinger_joint[i] -= add * 1.2;
		}
		else if (middlefinger_joint[i] < rock[i][2]) {
			middlefinger_joint[i] += add * 1.2;
		}
	}
	for (int i = 0; i < 3; i++) {
		if (abs(ringfinger_joint[i] - rock[i][3]) < add * 1) {
			times[i][3] += 1;
		}
		if (times[i][3]) {
			continue;
		}
		else {
			finish = false;
		}
		if (ringfinger_joint[i] > rock[i][3]) {
			ringfinger_joint[i] -= add * 1;
		}
		else if (ringfinger_joint[i] < rock[i][3]) {
			ringfinger_joint[i] += add * 1;
		}
	}
	for (int i = 0; i < 3; i++) {
		if (abs(littlefinger_joint[i] - rock[i][4]) < add * 1) {
			times[i][4] += 1;
		}
		if (times[i][4]) {
			continue;
		}
		else {
			finish = false;
		}
		if (littlefinger_joint[i] > rock[i][4]) {
			littlefinger_joint[i] -= add * 1;
		}
		else if (littlefinger_joint[i] < rock[i][4]) {
			littlefinger_joint[i] += add * 1;
		}
	}
	return finish;
}

bool Papper(double thumb_joint[3], double forefinger_joint[3], double middlefinger_joint[3], double ringfinger_joint[3], double littlefinger_joint[3], int** times) {
	bool finish = true;
	for (int i = 0; i < 3; i++) {
		if (abs(thumb_joint[i] - papper[i][0]) < add * 0.5) {
			thumb_joint[i] = papper[i][0];
			times[i][0] += 1;
		}
		if (times[i][0]) {
			continue;
		}
		else {
			finish = false;
		}
		if (thumb_joint[i] > papper[i][0]) {
			thumb_joint[i] -= add * 0.5;
		}
		else if (thumb_joint[i] < papper[i][0]) {
			thumb_joint[i] += add * 0.5;
		}
	}
	for (int i = 0; i < 3; i++) {
		if (abs(forefinger_joint[i] - papper[i][1]) < add * 1) {
			forefinger_joint[i] = papper[i][1];
			times[i][1] += 1;
		}
		if (times[i][1]) {
			continue;
		}
		else {
			finish = false;
		}
		if (forefinger_joint[i] > papper[i][1]) {
			forefinger_joint[i] -= add * 1;
		}
		else if (forefinger_joint[i] < papper[i][1]) {
			forefinger_joint[i] += add * 1;
		}
	}
	for (int i = 0; i < 3; i++) {
		if (abs(middlefinger_joint[i] - papper[i][2]) < add * 1) {
			middlefinger_joint[i] = papper[i][2];
			times[i][2] += 1;
		}
		if (times[i][2]) {
			continue;
		}
		else {
			finish = false;
		}
		if (middlefinger_joint[i] > papper[i][2]) {
			middlefinger_joint[i] -= add * 1;
		}
		else if (middlefinger_joint[i] < papper[i][2]) {
			middlefinger_joint[i] += add * 1;
		}
	}
	for (int i = 0; i < 3; i++) {
		if (abs(ringfinger_joint[i] - papper[i][3]) < add * 1) {
			ringfinger_joint[i] = papper[i][3];
			times[i][3] += 1;
		}
		if (times[i][3]) {
			continue;
		}
		else {
			finish = false;
		}
		if (ringfinger_joint[i] > papper[i][3]) {
			ringfinger_joint[i] -= add * 1;
		}
		else if (ringfinger_joint[i] < papper[i][3]) {
			ringfinger_joint[i] += add * 1;
		}
	}
	for (int i = 0; i < 3; i++) {
		if (abs(littlefinger_joint[i] - papper[i][4]) < add * 1) {
			littlefinger_joint[i] = papper[i][4];
			times[i][4] += 1;
		}
		if (times[i][4]) {
			continue;
		}
		else {
			finish = false;
		}
		if (littlefinger_joint[i] > papper[i][4]) {
			littlefinger_joint[i] -= add * 1;
		}
		else if (littlefinger_joint[i] < papper[i][4]) {
			littlefinger_joint[i] += add * 1;
		}
	}
	return finish;
}

void handShakeUp() {
	if (elbow > 9) {
		elbow -= 3;
	}
	else {
		type = false;
	}
	glutPostRedisplay();
}

void handShakeDown() {
	if (elbow < 30) {
		elbow += 3;
	}
	else {
		type = true;
		shakeTimes -= 1;
	}
	glutPostRedisplay();
}

int result(int player, int bot) {
	if (player == bot) {
		return 0;
	}
	switch (player) {
	case 1:
		if (bot == 2) {
			return -1;
		}
		else {
			return 1;
		}
	case 2:
		if (bot == 1) {
			return 1;
		}
		else {
			return -1;
		}
	case 3:
		if (bot == 1) {
			return -1;
		}
		else {
			return 1;
		}
	}
}

void myInit()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-50, 50, -50, 50, -50, 50);
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
}

void mykey(unsigned char key, int x, int y) {
	if (key == '1') {
		finger = 1;
	}
	else if (key == '2') {
		finger = 2;
	}
	else if (key == '3') {
		finger = 3;
	}
	else if (key == '4') {
		finger = 4;
	}
	else if (key == '5') {
		finger = 5;
	}
	if (key == '+') {
		add += 1;
	}
	else if (key == '-') {
		add -= 1;
	}
	if (key == 'A' || key == 'a') {
		switch (finger) {
		case 1:
			thumb_joint_1[0] += add;
			break;
		case 2:
			forefinger_joint_1[0] += add;
			break;
		case 3:
			middlefinger_joint_1[0] += add;
			break;
		case 4:
			ringfinger_joint_1[0] += add;
			break;
		case 5:
			littlefinger_joint_1[0] += add;
			break;
		default:
			break;
		}
	}
	else if (key == 'Z' || key == 'z') {
		switch (finger) {
		case 1:
			thumb_joint_1[0] -= add;
			break;
		case 2:
			forefinger_joint_1[0] -= add;
			break;
		case 3:
			middlefinger_joint_1[0] -= add;
			break;
		case 4:
			ringfinger_joint_1[0] -= add;
			break;
		case 5:
			littlefinger_joint_1[0] -= add;
			break;
		default:
			break;
		}
	}
	else if (key == 'S' || key == 's') {
		switch (finger) {
		case 1:
			thumb_joint_1[1] += add;
			break;
		case 2:
			forefinger_joint_1[1] += add;
			break;
		case 3:
			middlefinger_joint_1[1] += add;
			break;
		case 4:
			ringfinger_joint_1[1] += add;
			break;
		case 5:
			littlefinger_joint_1[1] += add;
			break;
		default:
			break;
		}
	}
	else if (key == 'X' || key == 'x') {
		switch (finger) {
		case 1:
			thumb_joint_1[1] -= add;
			break;
		case 2:
			forefinger_joint_1[1] -= add;
			break;
		case 3:
			middlefinger_joint_1[1] -= add;
			break;
		case 4:
			ringfinger_joint_1[1] -= add;
			break;
		case 5:
			littlefinger_joint_1[1] -= add;
			break;
		default:
			break;
		}
	}
	else if (key == 'D' || key == 'd') {
		switch (finger) {
		case 1:
			thumb_joint_1[2] += add;
			break;
		case 2:
			forefinger_joint_1[2] += add;
			break;
		case 3:
			middlefinger_joint_1[2] += add;
			break;
		case 4:
			ringfinger_joint_1[2] += add;
			break;
		case 5:
			littlefinger_joint_1[2] += add;
			break;
		default:
			break;
		}
	}
	else if (key == 'C' || key == 'c') {
		switch (finger) {
		case 1:
			thumb_joint_1[2] -= add;
			break;
		case 2:
			forefinger_joint_1[2] -= add;
			break;
		case 3:
			middlefinger_joint_1[2] -= add;
			break;
		case 4:
			ringfinger_joint_1[2] -= add;
			break;
		case 5:
			littlefinger_joint_1[2] -= add;
			break;
		default:
			break;
		}
	}
	if (key == '6') {
		for (int i = 0; i < 3; i++) {
			cout << thumb_joint_1[i] << "\t" << forefinger_joint_1[i] << "\t" << middlefinger_joint_1[i] << "\t";
			cout << ringfinger_joint_1[i] << "\t" << littlefinger_joint_1[i] << endl;
		}
		cout << "==========================================" << endl;
	}
	glutPostRedisplay();
}

void mymotion(int x, int y) {
	theta += (x - nowX) * 0.01;
	if (theta > 180 || theta < 0) {
		theta = 0;
	}
	phi += (y - nowY) * 0.01;
	if (phi > 360 || phi < 0) {
		phi = 0;
	}
	lookAtY = r * sin(theta) * cos(phi);
	lookAtX = r * sin(theta) * sin(phi);
	lookAtZ = r * cos(theta);
	nowX = x;
	nowY = y;
	glutPostRedisplay();
}