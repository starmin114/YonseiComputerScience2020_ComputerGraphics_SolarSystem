// HW1_2018193020
//      Mouse: Arcball manipulation
//      Keyboard: 'r' - reset arcball

#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <cmath>

#include <shader.h>
#include <cube.h>
#include <arcball.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <vector>
 

using namespace std;

float PI = acos(-1);

class Sphere {
public:
	
	float minEdgeArcLength = 1/36.0f;
	float radius;

	vector<float> vertices;
	vector<float> normals;
	vector<float> texCoords;
	vector<unsigned int> indices;

	unsigned int vsize, nsize, tsize;


	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;


	Sphere(float radius) {
		this->radius = radius;
		initCoords();
		initBuffers();
	};

	void initCoords() {
		cout << "initcoords Start" << endl;
		//Theta : about Y, Phi : about XZ.
		float dTheta = 1 / radius * minEdgeArcLength; //Theta resolution
		float cTheta = 1.0f / dTheta + 1; //Number of (xzPlane, y).
		float base = 0, end = 0; //For Indexing, cache lower Y's index offset.
		for (float theta = -0.5; theta < 0.5; theta += dTheta) {
			float xz = radius * cos(theta * PI);
			float y = radius * sin(theta * PI);
			float dPhi;
			if (xz == 0) {
				dPhi = INFINITY;
			}
			else {
				dPhi = 1 / abs(xz) * minEdgeArcLength;
			}
			
			int count = 0; //Number of (x, z) in xyPlane.
			
			for (float phi = 0; phi < 2; phi += dPhi) {
				float x = xz * cos(phi * PI);
				float z = xz * sin(phi * PI);
				vertices.push_back(x);
				vertices.push_back(y);
				vertices.push_back(z);
				normals.push_back(x);
				normals.push_back(y);
				normals.push_back(z);
				texCoords.push_back(phi / 2);
				texCoords.push_back(theta + 0.5);
				count++;
			}

			if (end != 0) {
				for (int i = base; i < end; i++) {
					int ii = i + 1;
					if (ii == end)
						ii = base;
					
					for (int j = end; j < end + count; j++) {
						int jj = j + 1;
						if (jj == end + count)
							jj = end;

						//higher, lower = 1,2
						if (count != 1) {
							indices.push_back(j);
							indices.push_back(i);
							indices.push_back(ii);
						}
						//higher, lower = 2,1
						if (end - count != 1) {
							indices.push_back(ii);
							indices.push_back(j);
							indices.push_back(jj);
						}
					}
				}
			}
			base = end;
			end = base + count;
		}
		
		//for (vector<float>::const_iterator i = texCoords.begin(); i != texCoords.end(); i = i + 2)
		//	cout << *i << ",  " << *(i + 1) << ",  " << " "  << "  " << endl;

		vsize = vertices.size();
		nsize = normals.size();
		tsize = texCoords.size();

	}

	void initBuffers() {


		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);



		// copy vertex attrib data to VBO
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, (vsize + nsize + tsize) * sizeof(float), 0, GL_STATIC_DRAW); // reserve space
		glBufferSubData(GL_ARRAY_BUFFER, 0, vsize * sizeof(float), &vertices[0]);                  // copy verts at offset 0
		glBufferSubData(GL_ARRAY_BUFFER, vsize * sizeof(float), nsize * sizeof(float), &normals[0]);               // copy norms after verts
		glBufferSubData(GL_ARRAY_BUFFER, (vsize + nsize) * sizeof(float), tsize * sizeof(float), &texCoords[0]); // copy texs after cols

		cout << "initBuffers Middle" << vsize << ", " << indices.size() << ", " << tsize <<endl;
		// copy index data to EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		

		// attribute position initialization
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);  // position attrib
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(vsize * sizeof(float))); // normal attrib
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)((vsize + nsize) * sizeof(float))); //tex attrib
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

	}

	void draw(Shader *shader) {
		//cout << "draw Start" << endl;k
		shader->use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		//cout << vsize << endl;
		glBindVertexArray(0);
		//cout << "draw Finish" << endl;
	};

};




// Function Prototypes
GLFWwindow *glAllInit();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action , int mods);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow *window, double x, double y);
unsigned int loadTexture(const char *);
void render();

// Global variables
GLFWwindow *mainWindow = NULL;
Shader *lightingShader = NULL;
unsigned int SCR_WIDTH = 600;
unsigned int SCR_HEIGHT = 600;
Sphere *earth, *sun, *moon;
glm::mat4 projection, view, model;

// for arcball
float arcballSpeed = 0.2f;
static Arcball camArcBall(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true );
bool arcballCamRot = true;

// for camera
glm::vec3 cameraPos(0.0f, 0.0f, 9.0f);

glm::vec3 lightSize(0.1f, 0.1f, 0.1f);

// direction of dirLights
glm::vec3 dirLightDirection(0.0f, -1.0f, 0.0f);
glm::vec3 dirLightDirection2(0.0f, 0.0f, -1.0f);
// positions of the lights
glm::vec3 pointLightPosition(0.0f, 0.0f, 0.0f);

// for texture
static unsigned int sunTexture, earthTexture, moonTexture;  // texture ids for diffuse and specular maps


int main()
{
    mainWindow = glAllInit();
    
    // shader loading and compile (by calling the constructor)
    lightingShader = new Shader("Sphere.vs", "Sphere.fs");
    
    // projection and view matrix
    lightingShader->use();
    projection = glm::perspective(glm::radians(45.0f),
                                  (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    lightingShader->setMat4("projection", projection);
    
    // load texture
	sunTexture = loadTexture("sun.jpg");
	earthTexture = loadTexture("earth.jpg");
	moonTexture = loadTexture("moon.jpg");
    
    // transfer texture id to fragment shader
    lightingShader->use();
    lightingShader->setFloat("shininess", 32);
    
    lightingShader->setVec3("viewPos", cameraPos);
    
    // transfer lighting parameters to fragment shader

	// dir light
	lightingShader->setVec3("dirLights[0].direction", dirLightDirection);
	lightingShader->setVec3("dirLights[0].ambient", glm::vec3(0.3f));
	lightingShader->setVec3("dirLights[0].diffuse", glm::vec3(0.4f));

	lightingShader->setVec3("dirLights[1].direction", dirLightDirection2);
	lightingShader->setVec3("dirLights[1].ambient", glm::vec3(0.1f));
	lightingShader->setVec3("dirLights[1].diffuse", glm::vec3(0.2f));


    // point light
	lightingShader->setVec3("pointLight.position", pointLightPosition);
    lightingShader->setVec3("pointLight.ambient", glm::vec3(1.0f));
    lightingShader->setVec3("pointLight.diffuse", glm::vec3(2.8f));
    lightingShader->setVec3("pointLight.specular", glm::vec3(4.0f));
    lightingShader->setFloat("pointLight.constant", 1.0f);
    lightingShader->setFloat("pointLight.linear", 0.09);
    lightingShader->setFloat("pointLight.quadratic", 0.032);

    
	sun = new Sphere(0.9f);
	earth = new Sphere(0.6f);
	moon = new Sphere(0.3f);
    
    while (!glfwWindowShouldClose(mainWindow)) {
        render();
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}

GLFWwindow *glAllInit()
{
    GLFWwindow *window;
    
    // glfw: initialize and configure
    if (!glfwInit()) {
        printf("GLFW initialisation failed!");
        glfwTerminate();
        exit(-1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // glfw window creation
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW1_2018193020", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    
    // OpenGL states
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    
    // Allow modern extension features
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cout << "GLEW initialisation failed!" << endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(-1);
    }
    
    return window;
}

unsigned int loadTexture(const char *texFileName){
    unsigned int texture;
    
    // Create texture ids.
    glGenTextures(1, &texture);
    
    // All upcomming GL_TEXTURE_2D operations now on "texture" object
    glBindTexture(GL_TEXTURE_2D, texture);
    
    // Set texture parameters for wrapping.
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);   // vertical flip the texture
    unsigned char *image = stbi_load(texFileName, &width, &height, &nrChannels, 0);
    if (!image) {
        printf("texture %s loading error ... \n", texFileName);
    }
    else printf("texture %s loaded\n", texFileName);
    
    GLenum format;
    if (nrChannels == 1) format = GL_RED;
    else if (nrChannels == 3) format = GL_RGB;
    else if (nrChannels == 4) format = GL_RGBA;
    
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexImage2D( GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image );
    glGenerateMipmap(GL_TEXTURE_2D);
    
    return texture;
}

void render() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
    view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotMat = camArcBall.createRotationMatrix();
	view = view * rotMat;

    // cube objects
    lightingShader->use();
    lightingShader->setMat4("view", view);
    
	lightingShader->setVec3("viewPos", glm::vec3(glm::vec4(cameraPos, 1.0f) * rotMat));

    // texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sunTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, earthTexture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, moonTexture);

	//animation
	float time = (float)glfwGetTime();

	//sun
	lightingShader->setInt("textureDiffuseMap", 0);
	lightingShader->setInt("type", 0);

	float sunRotSpeed = glm::radians(7.0f);
	model = glm::mat4(1.0f);
	model = glm::translate(model, pointLightPosition);
	model = glm::rotate(model, time * sunRotSpeed, glm::vec3(0.1f, 0.3f, 1.0f));
	lightingShader->setMat4("model", model);
	
	sun->draw(lightingShader);

	//earth
	lightingShader->setInt("textureDiffuseMap", 1);
	lightingShader->setInt("type", 1);
	
	float earthOrbitSpeed = glm::radians(50.0f);
	float earthRotSpeed = glm::radians(7.0f);
	float earthOrbitRad = 2.3f;
	glm::vec3 earthPosition = glm::vec3(cos(earthOrbitSpeed * time) * earthOrbitRad, sin(earthOrbitSpeed * time) * earthOrbitRad, 0.0f);
	model = glm::mat4(1.0f);
	model = glm::translate(model, earthPosition);
	model = glm::rotate(model, time * earthOrbitSpeed, glm::vec3(1.8f, 0.2f, 0.3f));
	lightingShader->setMat4("model", model);
	
	earth->draw(lightingShader);

	//moon
	lightingShader->setInt("textureDiffuseMap", 2);
	lightingShader->setInt("type", 2);
	
	float moonOrbitSpeed = glm::radians(90.0f);
	float moonRotSpeed = glm::radians(9.0f);
	float moonOrbitRad = 1.0f;
	glm::vec3 moonPosition = glm::vec3(cos(moonOrbitSpeed * time) * moonOrbitRad, sin(moonOrbitSpeed * time) * moonOrbitRad, 0.0f);
	model = glm::mat4(1.0f);
	model = glm::translate(model, moonPosition + earthPosition);
	model = glm::rotate(model, time * moonOrbitSpeed, glm::vec3(0.4f, 1.9f, 0.4f));
	lightingShader->setMat4("model", model);
	
	moon->draw(lightingShader);
 
    
    glfwSwapBuffers(mainWindow);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        camArcBall.init(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    camArcBall.mouseButtonCallback( window, button, action, mods );
}

void cursor_position_callback(GLFWwindow *window, double x, double y) {
    camArcBall.cursorCallback( window, x, y );
}
