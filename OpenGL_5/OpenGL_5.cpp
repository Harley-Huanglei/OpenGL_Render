#include <iostream>
#include "shader.h"
#include <glfw\glfw3.h>

int screenWidth = 1024;
int screenHeight = 540;

void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

#define TEXTURE_SETTING		\
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); \
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); \
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); \
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

int main()
{
	// glfw初始化
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_SAMPLES, 4);

	// 创建窗口
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Render_YUV", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad初始化
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// 获取顶点属性上限
	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

	// 顶点和纹理坐标
	static const GLfloat vertices[] = {
		-1.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		-1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	};

	// 索引缓冲
	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	// VBO VAO EBO
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	unsigned int EBO;
	glGenBuffers(1, &EBO);

	// 绑定VAO
	glBindVertexArray(VAO);

	// VAO绑定VBO，指定顶点属性
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 位置属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 纹理属性
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// VAO绑定EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// 解绑VAO
	glBindVertexArray(0);

	// 着色器程序设置纹理单元
	Shader shader("vertex.vs", "fragment.fs");
	shader.use();

	// 设置纹理1字节对齐
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// 读取纹理数据
	int width = 4096;
	int height = 2160;
	int size = (unsigned)(width * height * 1.5);
	unsigned char* data = new unsigned char[size];
	FILE* file = NULL;
	fopen_s(&file, "E:/data/4k.yuv", "rb");
	if (file)
	{
		fread(data, 1, size, file);
		fclose(file);
	}

	unsigned char* pY = data;
	unsigned char* pU = pY + width*height;
	unsigned char* pV = pU + width*height/4;

	int planeCount = 3;
	GLuint texture[3] = { 0 };
	glGenTextures(3, texture);

	// 纹理数据
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	TEXTURE_SETTING;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, pY);
	shader.setInt("tex_y", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	TEXTURE_SETTING;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width/2, height/2, 0, GL_RED, GL_UNSIGNED_BYTE, pU);
	shader.setInt("tex_u", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	TEXTURE_SETTING;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width/2, height/2, 0, GL_RED, GL_UNSIGNED_BYTE, pV);
	shader.setInt("tex_v", 2);

	// 启用深度测试
	glEnable(GL_DEPTH_TEST);

	// 线框模式
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window))
	{
		// 事件处理
		processInput(window);

		// 绑定VAO
		glBindVertexArray(VAO); 

		// 渲染
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// VAO绘制
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		// EBO绘制
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// swap buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// 键盘输入
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}