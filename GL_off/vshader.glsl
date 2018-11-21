#version 330 core

in vec3 vPosition;
in vec3 vColor;
out vec3 color;
uniform mat4 matrix;

void main()
{
	//���һ������ʵ�����š���ת��ƽ�Ʊ任
    gl_Position = matrix * vec4(vPosition, 1.0);  //gl_Position�����ö���������ᴫ����դ��ģ����
    color = vColor;
}
