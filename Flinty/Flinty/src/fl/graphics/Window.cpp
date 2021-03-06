#include "Window.h"

#include <GLFW/glfw3.h>
#include "fl/system/Timer.h"

namespace fl {

	static void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void GLFWWindowSizeCallback(GLFWwindow* window, int width, int height);
	static void EmptyFunction() {}
	static void EmptyRenderFunction(Renderer&) {}
	static void EmptyKeyCallback(int, int) {}

	Window::Window(const String& title, uint width, uint height)
		: m_Title(title), m_Width(width), m_Height(height), m_FrameTime(0.0f)
	{
		m_OnUpdate = EmptyFunction;
		m_OnRender = EmptyRenderFunction;
		m_KeyCallback = EmptyKeyCallback;

		Init();
	}

	Window::~Window()
	{
	}

	void Window::Init()
	{
		glfwInit();
		m_GLFWWindow = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), NULL, NULL);
		GLFWwindow* window = m_GLFWWindow;
		glfwSetWindowUserPointer(window, this);
		glfwSetKeyCallback(window, GLFWKeyCallback);
		glfwSetWindowSizeCallback(window, GLFWWindowSizeCallback);
		glfwMakeContextCurrent(window);
		m_Renderer = new Renderer();
		Renderer::Init();
	}

	void Window::Show()
	{
		// m_GraphicsThread = std::thread(std::bind(&Window::GraphicsThread, this));
		GraphicsThread();
	}

	void Window::GraphicsThread()
	{
		GLFWwindow* window = m_GLFWWindow;
		float updateTimer = 0.0f;
		float updateTick = 1.0f / 60.0f;
		uint frames = 0;
		uint updates = 0;
		Timer timer;
		while (!glfwWindowShouldClose(window))
		{
			float now = timer.ElapsedMillis();

			Clear();

			if (now - updateTimer > updateTick)
			{
				m_OnUpdate();
				updates++;
				updateTimer += updateTick;
			}

			{
				Timer frametime;
				m_OnRender(*m_Renderer);
				frames++;
				m_FrameTime = frametime.ElapsedMillis();
			}
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
		glfwTerminate();
	}
	
	void Window::Clear()
	{
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}

	static void GLFWKeyCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods)
	{
		Window* window = (Window*)glfwGetWindowUserPointer(glfwWindow);
		window->m_KeyCallback(key, action);
		if (action == GLFW_PRESS)
			window->m_PressedKeys.insert(key);
		else if (action == GLFW_RELEASE)
			window->m_PressedKeys.erase(key);
	}

	static void GLFWWindowSizeCallback(GLFWwindow* glfwWindow, int width, int height)
	{
		Window* window = (Window*)glfwGetWindowUserPointer(glfwWindow);
		window->m_Width = width;
		window->m_Height = height;

		GLCall(glViewport(0, 0, width, height));
	}
}
