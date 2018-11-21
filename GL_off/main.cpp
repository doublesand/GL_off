#include "Angel.h"
#include <vector>
#include <string>
#include <fstream>

#pragma comment(lib, "glew32.lib")

using namespace std;

// ��¼������Ƭ�еĶ������еĽṹ��
typedef struct vIndex {
	unsigned int a, b, c;
	vIndex(int ia, int ib, int ic) : a(ia), b(ib), c(ic) {}
} vec3i;

vector<vec3>  vertices;
vector<vec3i> faces;

vector<vec3> points;    //���붥����ɫ���ĵ�
vector<vec3> colors;    //����ƬԪ��ɫ���ĵ�

int nFaces;
int nVertices;
int nEdges;

const vec3 vertex_colors[8] = {
	vec3(1.0, 1.0, 1.0),  // White
	vec3(1.0, 1.0, 0.0),  // Yellow
	vec3(0.0, 1.0, 0.0),  // Green
	vec3(0.0, 1.0, 1.0),  // Cyan
	vec3(1.0, 0.0, 1.0),  // Magenta
	vec3(1.0, 0.0, 0.0),  // Red
	vec3(0.0, 0.0, 0.0),  // Black
	vec3(0.0, 0.0, 1.0)   // Blue
};

//----------------------------------���š�ƽ�ơ���ת��Ҫ�ı���
const int X_AXIS = 0;
const int Y_AXIS = 1;
const int Z_AXIS = 2;

const int TRANSFORM_SCALE = 0;
const int TRANSFORM_ROTATE = 1;
const int TRANSFORM_TRANSLATE = 2;

const double DELTA_DELTA = 0.1;        // Delta�ı仯��
const double DEFAULT_DELTA = 0.3;      // Ĭ�ϵ�Deltaֵ

double scaleDelta = DEFAULT_DELTA;
double rotateDelta = DEFAULT_DELTA;
double translateDelta = DEFAULT_DELTA;

vec3 scaleTheta(1.0, 1.0, 1.0);        // ���ſ��Ʊ���
vec3 rotateTheta(0.0, 0.0, 0.0);       // ��ת���Ʊ���
vec3 translateTheta(0.0, 0.0, 0.0);    // ƽ�ƿ��Ʊ���

GLuint mainWin;                                //������
GLint matrixLocation;                          //����λ��
int currentTransform = TRANSFORM_TRANSLATE;    // ��ǰ���ĸ�����ת

//-----------------------------------------------������ʵ��
// ��ԭTheta��Delta
void resetTheta()
{
	scaleTheta = vec3(1.0, 1.0, 1.0);
	rotateTheta = vec3(0.0, 0.0, 0.0);
	translateTheta = vec3(0.0, 0.0, 0.0);
	scaleDelta = DEFAULT_DELTA;
	rotateDelta = DEFAULT_DELTA;
	translateDelta = DEFAULT_DELTA;
}

void read_off(string filename) {
	if (filename.empty()) return;

	ifstream fin;
	fin.open(filename);
	if (!fin) {
		cout << "�ļ���ȡ����" << endl;
		return;
	}
	//����off�ļ���ʽ��ȡ����
	cout << "�ļ��򿪳ɹ�" << endl;
	//��ȡ�ַ���OFF
	string str;
	fin >> str;
	//��ȡ�ļ��еĶ���������Ƭ��������
	fin >> nVertices >> nFaces >> nEdges;
	// ���ݶ�������ѭ����ȡÿ����������
	for (int i = 0; i < nVertices; i++) {
		vec3 node;  
		fin >> node.x >> node.y >> node.z;  //ע��������С��
		vertices.push_back(node);
	}
	// ������Ƭ����ѭ����ȡÿ����Ƭ��Ϣ�����ù�����vec3i�ṹ�屣��
	for (int i = 0; i < nFaces; i++) {
		int num, a, b, c;
		fin >> num >> a >> b >> c;
		faces.push_back(vec3i(a, b, c));
	}
	fin.close();
}

void storeFacesPoints()
{
	points.clear();
	colors.clear();
	// ��points��colors�����д洢ÿ��������Ƭ�ĸ��������ɫ��Ϣ
	for (int i = 0; i < nFaces; i++)
	{
		// ��points�����У��������ÿ����Ƭ�Ķ��㣬����colors�����У���Ӹõ����ɫ��Ϣ
		points.push_back(vertices[faces[i].a]);
		colors.push_back(vertex_colors[i/2]);

		points.push_back(vertices[faces[i].b]);
		colors.push_back(vertex_colors[i/2]);

		points.push_back(vertices[faces[i].c]);
		colors.push_back(vertex_colors[i/2]);

	}
}

void init() {
	//��OFF�ļ��е�����ת�ɹ�������
	storeFacesPoints();  
	
	//���������������
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//��������ʼ�����㻺�����
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(vec3) + colors.size()*sizeof(vec3), NULL, GL_STATIC_DRAW);

	//��ȡ�������ɫ����������ݵ��ڴ���
	glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(vec3), &points[0]);
	glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(vec3), colors.size() * sizeof(vec3), &colors[0]);
	
	//��ȡ��ɫ����ʹ��
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	//�Ӷ�����ɫ���г�ʼ�������λ��
	GLuint pLocation = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(pLocation);
	glVertexAttribPointer(pLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	//��ƬԪ��ɫ���г�ʼ���������ɫ
	GLuint cLocation = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(cLocation);
	glVertexAttribPointer(cLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(points.size() * sizeof(vec3)));

	// ��þ���洢λ��
	matrixLocation = glGetUniformLocation(program, "matrix");  

	//��ɫ����
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void display() {   //������õ�ƬԪ��ɫ��
	//�����ڣ�������ɫ�������Ȼ���
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//����һ����λ���󣬹�����
	mat4 m(1.0, 0.0, 0.0, 0.0,
		   0.0, 1.0, 0.0, 0.0,
		   0.0, 0.0, 1.0, 0.0,
		   0.0, 0.0, 0.0, 1.0);

	// ���ú����������ֱ仯�ı仯��������仯����


	//------------------�������ע�Ϳ��Դ򿪿�����ʲô��ϲ---------------//
	// ���Ʊ�
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// �����������
	// �������޳�����
	//glEnable(GL_CULL_FACE);
	// �޸��޳����������-GL_BACK & GL_FRONT
	//glCullFace(GL_FRONT);
	//-------------------------------------------------------------------//

	//���ƶ���
	glDrawArrays(GL_TRIANGLES, 0, points.size()); //ÿ������Ϊһ�������Σ�����������
	glutSwapBuffers();
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);

	//������ʾģʽ֧����Ȳ���
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("��ȡoff�ļ�����3Dģ��");

	//ϣ���ڵ����κ�OpenGL����ǰ��ʼ��GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	//��ȡoff�ļ�
	read_off("cube.off");

	init();
	glutDisplayFunc(display);

	//������Ȳ���
	glEnable(GL_DEPTH_TEST);

	glutMainLoop();
	return 0;
}