//
// Author: Ahmet Oguz Akyuz
// 
// This is a sample code that draws a single block piece at the center
// of the window. It does many boilerplate work for you -- but no
// guarantees are given about the optimality and bug-freeness of the
// code. You can use it to get started or you can simply ignore its
// presence. You can modify it in any way that you like.
//
//


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#define _USE_MATH_DEFINES
#include <math.h>
#include <GL/glew.h>
//#include <OpenGL/gl3.h>   // The GL Header File
#include <GLFW/glfw3.h> // The GLFW header
#include <glm/glm.hpp> // GL Math library header
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <ft2build.h>
#include FT_FREETYPE_H

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

#define GROUND_LEVEL -6.0f
#define TETRIS_START_LEVEL 6.0f
#define GRID_RESOLUTION 9

using namespace std;

GLuint gProgram[4];
int gWidth = 600, gHeight = 1000;
GLuint gVertexAttribBuffer, gTextVBO, gIndexBuffer;
GLuint gTex2D;
int gVertexDataSizeInBytes, gNormalDataSizeInBytes;
int gTriangleIndexDataSizeInBytes, gLineIndexDataSizeInBytes;
int vertexPosCubeSizeInBytes, groundUnitVertexSizeInBytes;
int gIndicesCubeSizeInBytes, gIndicesGroundUnitSizeInBytes;
int gIndicesCubeLinesSizeInBytes, gIndicesGroundUnitLinesSizeInBytes;

GLint modelingMatrixLoc[3];
GLint viewingMatrixLoc[3];
GLint projectionMatrixLoc[3];
GLint eyePosLoc[3];
GLint lightPosLoc[3];
GLint kdLoc[3];

glm::mat4 projectionMatrix;
glm::mat4 viewingMatrix;
glm::mat4 modelingMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, 0));
glm::vec3 eyePos = glm::vec3(0, 3, 25);
glm::vec3 lightPos = glm::vec3(0, 18, 40);

//glm::vec3 kdGround(0.334, 0.288, 0.635); // this is the ground color in the demo
glm::vec3 kdCubes(0.06, 0.80, 0.80);
glm::vec3 kdGround(0.03, 0.20, 0.89);

// button states
bool pressedA = false, pressedD = false, pressedS = false, pressedW = false, pressedH = false, pressedK = false; 

int grid[GRID_RESOLUTION][15][GRID_RESOLUTION];

int activeProgramIndex = 0;

// For Falling Objects
float curObjX = 0, curObjZ = 0;
float lastFallTime = 0.0, intervalForFalling = 0.0, curLevel = TETRIS_START_LEVEL ;

bool game_over = false;

// level ve puanlama
int point = 0;

// Current View
enum View {
    FRONT, //  0
    RIGHT, //  1
    BACK,  //  2
    LEFT   //  3
};

int curView = FRONT;

// For rotation angle
float curAngle = 0.0f, angleLimit = 0.0f;
bool decreaseAngleVar = false, increaseAngleVar = false; 


// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;

// For reading GLSL files
bool ReadDataFromFile(
    const string& fileName, ///< [in]  Name of the shader file
    string&       data)     ///< [out] The contents of the file
{
    fstream myfile;

    // Open the input 
    myfile.open(fileName.c_str(), std::ios::in);

    if (myfile.is_open())
    {
        string curLine;

        while (getline(myfile, curLine))
        {
            data += curLine;
            if (!myfile.eof())
            {
                data += "\n";
            }
        }

        myfile.close();
    }
    else
    {
        return false;
    }

    return true;
}

GLuint createVS(const char* shaderName)
{
    string shaderSource;

    string filename(shaderName);
    if (!ReadDataFromFile(filename, shaderSource))
    {
        cout << "Cannot find file name: " + filename << endl;
        exit(-1);
    }

    GLint length = shaderSource.length();
    const GLchar* shader = (const GLchar*) shaderSource.c_str();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &shader, &length);
    glCompileShader(vs);

    char output[1024] = {0};
    glGetShaderInfoLog(vs, 1024, &length, output);
    printf("VS compile log: %s\n", output);

	return vs;
}

GLuint createFS(const char* shaderName)
{
    string shaderSource;

    string filename(shaderName);
    if (!ReadDataFromFile(filename, shaderSource))
    {
        cout << "Cannot find file name: " + filename << endl;
        exit(-1);
    }

    GLint length = shaderSource.length();
    const GLchar* shader = (const GLchar*) shaderSource.c_str();

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &shader, &length);
    glCompileShader(fs);

    char output[1024] = {0};
    glGetShaderInfoLog(fs, 1024, &length, output);
    printf("FS compile log: %s\n", output);

	return fs;
}

void initFonts(int windowWidth, int windowHeight)
{
    // Set OpenGL options
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(windowWidth), 0.0f, static_cast<GLfloat>(windowHeight));
    glUseProgram(gProgram[3]);
    glUniformMatrix4fv(glGetUniformLocation(gProgram[3], "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    }

    // Load font as face
    FT_Face face;
    if (FT_New_Face(ft, "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 0, &face))
    //if (FT_New_Face(ft, "/usr/share/fonts/truetype/gentium-basic/GenBkBasR.ttf", 0, &face)) // you can use different fonts
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    }

    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

    // Load first 128 characters of ASCII set
    for (GLubyte c = 0; c < 128; c++)
    {
        // Load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
                );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            (GLuint) face->glyph->advance.x
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    //
    // Configure VBO for texture quads
    //
    glGenBuffers(1, &gTextVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gTextVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void initShaders()
{
	// Create the programs

    gProgram[0] = glCreateProgram(); // Cube shader by Oğuz hoca but I made it ground shader
	gProgram[1] = glCreateProgram(); // Line shader by Oğuz hoca
    gProgram[2] = glCreateProgram(); // Cube shader by me
    gProgram[3] = glCreateProgram(); // Text shader by Oğuz hoca


	// Create the shaders for both programs

    GLuint vs1 = createVS("vert.glsl"); // for cube shading
    GLuint fs1 = createFS("frag.glsl");

	GLuint vs2 = createVS("vert2.glsl"); // for border shading
	GLuint fs2 = createFS("frag2.glsl");

	GLuint vs4 = createVS("vert_text.glsl");  // for text shading
	GLuint fs4 = createFS("frag_text.glsl");

    GLuint vs3 = createVS("vert3.glsl");  // for text shading
	GLuint fs3 = createFS("frag3.glsl");

	// Attach the shaders to the programs

	glAttachShader(gProgram[0], vs1);
	glAttachShader(gProgram[0], fs1);

	glAttachShader(gProgram[1], vs2);
	glAttachShader(gProgram[1], fs2);

	glAttachShader(gProgram[2], vs3);
	glAttachShader(gProgram[2], fs3);

    glAttachShader(gProgram[3], vs4);
	glAttachShader(gProgram[3], fs4);

	// Link the programs

    for (int i = 0; i < 4; ++i)
    {
        glLinkProgram(gProgram[i]);
        GLint status;
        glGetProgramiv(gProgram[i], GL_LINK_STATUS, &status);

        if (status != GL_TRUE)
        {
            cout << "Program link failed: " << i << endl;
            exit(-1);
        }
    }


	// Get the locations of the uniform variables from both programs

	for (int i = 0; i < 3; ++i)
	{
		modelingMatrixLoc[i] = glGetUniformLocation(gProgram[i], "modelingMatrix");
		viewingMatrixLoc[i] = glGetUniformLocation(gProgram[i], "viewingMatrix");
		projectionMatrixLoc[i] = glGetUniformLocation(gProgram[i], "projectionMatrix");
		eyePosLoc[i] = glGetUniformLocation(gProgram[i], "eyePos");
		lightPosLoc[i] = glGetUniformLocation(gProgram[i], "lightPos");
		kdLoc[i] = glGetUniformLocation(gProgram[i], "kd");

        glUseProgram(gProgram[i]);
        glUniformMatrix4fv(modelingMatrixLoc[i], 1, GL_FALSE, glm::value_ptr(modelingMatrix));
        glUniform3fv(eyePosLoc[i], 1, glm::value_ptr(eyePos));
        glUniform3fv(lightPosLoc[i], 1, glm::value_ptr(lightPos));
        
        if(i == 2){
            glUniform3fv(kdLoc[i], 1, glm::value_ptr(kdGround));
            if (kdLoc[2] == -1) {
                std::cerr << "Uniform 'kd' not found in gProgram[2]" << std::endl;
            }
        }
        else 
            glUniform3fv(kdLoc[i], 1, glm::value_ptr(kdCubes));
	}
}

// VBO setup for drawing a cube and its borders
void initVBO()
{
    GLuint vao;
    glGenVertexArrays(1, &vao);
    assert(vao > 0);
    glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	assert(glGetError() == GL_NONE);

	glGenBuffers(1, &gVertexAttribBuffer);
	glGenBuffers(1, &gIndexBuffer);

	assert(gVertexAttribBuffer > 0 && gIndexBuffer > 0);

	glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);

    GLuint indicesCube[] = {
        0, 1, 2, // front
        3, 0, 2, // front
        4, 7, 6, // back
        5, 4, 6, // back
        0, 3, 4, // left
        3, 7, 4, // left
        2, 1, 5, // right
        6, 2, 5, // right
        3, 2, 7, // top
        2, 6, 7, // top
        0, 4, 1, // bottom
        4, 5, 1  // bottom
    };

    GLuint indicesGroundUnit[] = {
        0, 1, 2, // front
        3, 0, 2, // front
        4, 7, 6, // back
        5, 4, 6, // back
        0, 3, 4, // left
        3, 7, 4, // left
        2, 1, 5, // right
        6, 2, 5, // right
        3, 2, 7, // top
        2, 6, 7, // top
        0, 4, 1, // bottom
        4, 5, 1  // bottom
    };

    for(int i = 0; i < 36 ; i++) indicesGroundUnit[i] += (8);

    GLuint indicesCubeLines[] = {
        7, 3, 2, 6, // top
        4, 5, 1, 0, // bottom
        2, 1, 5, 6, // right
        5, 4, 7, 6, // back
        0, 1, 2, 3, // front
        0, 3, 7, 4, // left
    };

    GLuint indicesGroundUnitLines[] = {
        7, 3, 2, 6, // top
        4, 5, 1, 0, // bottom
        2, 1, 5, 6, // right
        5, 4, 7, 6, // back
        0, 1, 2, 3, // front
        0, 3, 7, 4, // left
    };

    for(int i = 0; i < 24 ; i++) indicesGroundUnitLines[i] += (8);

    GLfloat vertexPosCube[] = {
        0, 0, 1, // 0: bottom-left-front
        1, 0, 1, // 1: bottom-right-front
        1, 1, 1, // 2: top-right-front
        0, 1, 1, // 3: top-left-front
        0, 0, 0, // 4: bottom-left-back
        1, 0, 0, // 5: bottom-right-back
        1, 1, 0, // 6: top-right-back
        0, 1, 0, // 7: top-left-back
    };

    GLfloat groundUnitVertex[] = {
        0, 0.5, 1,   // 0: bottom-left-front
        1, 0.5, 1,   // 1: bottom-right-front
        1, 1, 1, // 2: top-right-front
        0, 1, 1, // 3: top-left-front
        0, 0.5, 0,   // 4: bottom-left-back
        1, 0.5, 0,   // 5: bottom-right-back
        1, 1, 0, // 6: top-right-back
        0, 1, 0, // 7: top-left-back
    };

    GLfloat vertexNorCube[] = {
         1.0,  1.0,  1.0, // 0: unused
         0.0, -1.0,  0.0, // 1: bottom
         0.0,  0.0,  1.0, // 2: front
         1.0,  1.0,  1.0, // 3: unused
        -1.0,  0.0,  0.0, // 4: left
         1.0,  0.0,  0.0, // 5: right
         0.0,  0.0, -1.0, // 6: back 
         0.0,  1.0,  0.0, // 7: top
         // Below is for ground unit
         1.0,  1.0,  1.0, // 0: unused
         0.0, -1.0,  0.0, // 1: bottom
         0.0,  0.0,  1.0, // 2: front
         1.0,  1.0,  1.0, // 3: unused
        -1.0,  0.0,  0.0, // 4: left
         1.0,  0.0,  0.0, // 5: right
         0.0,  0.0, -1.0, // 6: back 
         0.0,  1.0,  0.0, // 7: top
    };

    vertexPosCubeSizeInBytes = sizeof(vertexPosCube);
    groundUnitVertexSizeInBytes = sizeof(groundUnitVertex);

	gVertexDataSizeInBytes = vertexPosCubeSizeInBytes + groundUnitVertexSizeInBytes;
	gNormalDataSizeInBytes = sizeof(vertexNorCube);

    gIndicesCubeSizeInBytes = sizeof(indicesCube);
    gIndicesGroundUnitSizeInBytes = sizeof(indicesGroundUnit);

    gTriangleIndexDataSizeInBytes = gIndicesCubeSizeInBytes + gIndicesGroundUnitSizeInBytes;

    gIndicesCubeLinesSizeInBytes = sizeof(indicesCubeLines);
    gIndicesGroundUnitLinesSizeInBytes = sizeof(indicesGroundUnitLines);

    gLineIndexDataSizeInBytes = gIndicesCubeLinesSizeInBytes +  gIndicesGroundUnitLinesSizeInBytes  ;

    int allIndexSize = gTriangleIndexDataSizeInBytes + gLineIndexDataSizeInBytes;

    // vertexler: küp vertexleri, ground unit vertexleri, normal
	glBufferData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes + gNormalDataSizeInBytes, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertexPosCubeSizeInBytes, vertexPosCube);
    glBufferSubData(GL_ARRAY_BUFFER, vertexPosCubeSizeInBytes, groundUnitVertexSizeInBytes, groundUnitVertex);
	glBufferSubData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes, gNormalDataSizeInBytes, vertexNorCube);

    // indexler: küp indexler, ground unit indexleri, normal
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, allIndexSize, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, gIndicesCubeSizeInBytes, indicesCube); // add cube indices
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, gIndicesCubeSizeInBytes, gIndicesGroundUnitSizeInBytes, indicesGroundUnit); // add ground unit indices
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, gTriangleIndexDataSizeInBytes, gIndicesCubeLinesSizeInBytes, indicesCubeLines);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, gTriangleIndexDataSizeInBytes + gIndicesCubeLinesSizeInBytes , gIndicesGroundUnitLinesSizeInBytes, indicesGroundUnitLines);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexDataSizeInBytes));
}

void init() 
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // polygon offset is used to prevent z-fighting between the cube and its borders
    glPolygonOffset(0.5, 0.5);
    glEnable(GL_POLYGON_OFFSET_FILL);

    initShaders();
    initVBO();
    initFonts(gWidth, gHeight);
}

void drawCubeEdges(glm::mat4 I, int offset)
{
    glLineWidth(3.1);

    glUseProgram(gProgram[1]);

    glUniformMatrix4fv(modelingMatrixLoc[1], 1, GL_FALSE, glm::value_ptr(I));

    for (int i = 0; i < 6; ++i)
    {
	    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(offset +  i * 4 * sizeof(GLuint)));
    }
}

void drawGround()
{
    float xStep = -4.5, zStep = -4.5;

    for(int i = 0; i < GRID_RESOLUTION ; i++)
    {
        for(int j = 0; j < GRID_RESOLUTION ; j++)
        {
            glm::mat4 I = glm::mat4(1.0f); // identity matrixi olusturdum

            I = glm::translate(I, glm::vec3(xStep, GROUND_LEVEL, zStep) ); // stepler kadar translate ettim

            glUseProgram(gProgram[0]); // suanki program olarak ayarladim

            glUniformMatrix4fv(modelingMatrixLoc[0], 1, GL_FALSE, glm::value_ptr(I));

            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT,BUFFER_OFFSET( gIndicesCubeSizeInBytes));

            drawCubeEdges(I, gTriangleIndexDataSizeInBytes + gIndicesCubeLinesSizeInBytes );

            xStep += 1.0;
        }
        xStep = -4.5;
        zStep += 1.0;
    }
}


void renderText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    // Activate corresponding render state	
    glUseProgram(gProgram[3]);
    glUniform3f(glGetUniformLocation(gProgram[3], "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) 
    {
        Character ch = Characters[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;

        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },            
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }           
        };

        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, gTextVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

        //glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)

        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

void fall()
{
    if(intervalForFalling == 0.0 || game_over) return; // the game has not started yet

    if(glfwGetTime() - lastFallTime > intervalForFalling )
    {
        curLevel -= 1;
        lastFallTime = glfwGetTime();
    }

}

void drawCurrentObject()
{
    if(game_over){
        renderText("GAME OVER!", 20  , gHeight/2 , 1.75, glm::vec3(1, 1, 0));
        return;
    }
    fall(); // fall the object

    glm::mat4 I = glm::mat4(1.0f); // identity
    
    float xStepInit =  curObjX -1.5, zStepInit =  curObjZ - 1.5;
    float xStep =  xStepInit,  zStep = zStepInit, yStep = curLevel;
    // We need to draw 3x3x3 cube    
    for(int i=0; i < 3; i++) // y coordinate
    {

        for(int j = 0; j < 3 ; j++) // z coordinate
        {
            for(int k = 0; k < 3 ; k++) // x coordiante
            {
                glUseProgram(gProgram[2]);

                I = glm::mat4(1.0f);
                I = glm::translate(I, glm::vec3(xStep, yStep, zStep ));

                glUniformMatrix4fv(modelingMatrixLoc[2], 1, GL_FALSE, glm::value_ptr(I));

                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
                xStep += 1;

                drawCubeEdges(I, gTriangleIndexDataSizeInBytes );
            }
            xStep =  xStepInit;
            zStep += 1;
            
        }
        zStep = zStepInit;
        yStep += 1;
    }

}

void drawBox(float x, float y, float z)
{   

    glm::mat4 I = glm::mat4(1.0f); // identity matrixi olusturdum

    I = glm::translate(I, glm::vec3(x, y , z ) ); // stepler kadar translate ettim
    
    glUseProgram(gProgram[2]);

    glUniformMatrix4fv(modelingMatrixLoc[2], 1, GL_FALSE, glm::value_ptr(I));

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT,BUFFER_OFFSET( 0));

    drawCubeEdges(I, gTriangleIndexDataSizeInBytes );
    
}


void drawScene()
{
    for(int i = 0; i < 9 ; i++) // X
    {
        for(int j = 0; j < 9 ; j++ ) // Z
        {
            for(int k = 0; k < 15 ; k++) // K's are from -5 to 5. Mapping is [-5,5] -> [0,10]
            {
                if(grid[i][k][j])
                {
                    drawBox(i - 4.5 , k - 5  ,  j - 4.5 );
                }
            }
        }
    }
}

void resetObject()
{
    curObjX = curObjZ = 0; curLevel = TETRIS_START_LEVEL;
}

void saveCurObject()
{
    
    for(int i = -1; i < 2 ; i++) // X
    {
        for(int j = -1; j < 2 ; j++) // Z
        {
            for(int k = 0; k < 3 ; k++) // Y
            {
                grid[(int)curObjX + 4 + i ] [(int)curLevel + 4 + k] [(int)curObjZ + 4 + j] = 1; // mapping from [-4,4] to [0,8] to prevent seg fault
            }
        }
    }

    resetObject();
}

void checkCollisionWithGround()
{
    if( curLevel == GROUND_LEVEL + 2 ){ // cur level can be at most -4 because ground is from -6 to -5
        std::cout << "REACHED THE GROUND! " << std::endl;
        saveCurObject();
    }
}

void stopSystem()
{
    game_over = true;
}

void checkCollisionWithObjects(){
    int below_level = curLevel - 1;
    for(int i = -1; i < 2 ;  i++)
    {
        for(int j = -1; j < 2 ; j++)
        {
            if(grid[ (int)curObjX + 4 + i][below_level + 4][ (int)curObjZ + 4 + j ]){
                //std::cout << "UPPER LEVEL: " << below_level << std::endl; // for debug
                if( below_level > 1 )
                    stopSystem();// stop rendering an object
                if(curLevel != TETRIS_START_LEVEL)
                    saveCurObject();
            }
        }
    }
}

void checkCollission()
{
    checkCollisionWithGround();
    //return;
    checkCollisionWithObjects();
}

void renderView()
{
    GLfloat x, y;
    x = gWidth - (gWidth * 0.99);
    y = gHeight - gHeight/25;
    switch(curView)
    {
        case 0:
            renderText("Front", x , y , 0.75, glm::vec3(1, 1, 0)); break;
        case 1:
            renderText("Right", x  , y, 0.75, glm::vec3(1, 1, 0)); break;
        case 2:
            renderText("Back", x  , y , 0.75, glm::vec3(1, 1, 0)); break;
        case 3:
            renderText("Left", x  , y, 0.75, glm::vec3(1, 1, 0)); break;
        default: break;
    }
}

void turn90()
{
    glm::mat4 I = glm::mat4(1.0f);

    glm::vec3 eye, light;

    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(curAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    eye = glm::vec3(rotation * glm::vec4(eyePos, 1.0f)); // Rotate the eye position
    light = glm::vec3(rotation * glm::vec4(lightPos, 1.0f));

    for (int i = 0; i < 3; ++i)
    {
        glUseProgram(gProgram[i]);
        glm::mat4 viewingMatrix = glm::lookAt(eye, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(viewingMatrixLoc[i], 1, GL_FALSE, glm::value_ptr(viewingMatrix));
        glUniform3fv(lightPosLoc[i], 1, glm::value_ptr(light));
    }
    if(increaseAngleVar){
        if(curAngle <= angleLimit)   curAngle += 2;
        else increaseAngleVar = false;
        if(  curAngle >= 360) curAngle = angleLimit = 0.0f;
    }
    if(decreaseAngleVar){
        if(curAngle >= angleLimit)   curAngle -= 2;
        else decreaseAngleVar = false;
        if(  curAngle >= 360) curAngle = angleLimit = 0.0f;
    }
    //std::cout << "curAngle:" << curAngle << std::endl;
}

void resetGrid()
{
    int tempGrid[GRID_RESOLUTION][15][GRID_RESOLUTION];

    for(int i = 0; i < GRID_RESOLUTION ; i++)
    {
        for(int j = 0; j < GRID_RESOLUTION ; j++)
        {
            for(int k = 0 ; k < 15 ; k++)
            {
                tempGrid[i][k][j] = grid[i][k][j];
                grid[i][k][j] = 0; // make each cell zero to restore back clearly
            }
        }
    }

    for(int i = 0; i < GRID_RESOLUTION ; i++)
    {
        for(int j = 0; j < GRID_RESOLUTION ; j++)
        {
            for(int k = 3;  k < 15 ; k++ )
            {
                if(tempGrid[i][k][j]) grid[i][k-3][j] = 1;
            }
        }
    }
    point += 243;
}

void checkLevelCollapse(){
    // TO DO 
    // arrange points with respect to collaped part
    
    bool noEmpty = false;

    for(int i = 0; i < GRID_RESOLUTION ; i++)
    {
        for(int j = 0; j < GRID_RESOLUTION ; j++)
        {
            for(int k = 0 ; k < 3 ; k++)
            {
                if( grid[i][k][j] == 0 ) {
                    //std:cout << "i:" << i << " k:" << k << " j:" << j << std::endl;
                    noEmpty = true; // there is an empty space in grid
                }
            }
        }
    }

    if(noEmpty) return;
    std::cout << "COLLAPSE!" << std::endl;
    resetGrid();
}

void showPressedButton()
{
    GLfloat x, y;
    x = gWidth - (gWidth * 0.99);
    y = gHeight - gHeight/15;

    if(pressedA)
    {
        renderText("A",  x  , y, 0.45, glm::vec3(0.6, 0, 1));
    }
    else if(pressedS)
    {
        renderText("S" ,  x  , y, 0.45, glm::vec3(0.6, 0, 1));
    }
    else if(pressedD)
    {
        renderText("D",  x  , y, 0.45, glm::vec3(0.6, 0, 1));
    }
    else if(pressedW)
    {
        renderText("W",  x  , y , 0.45, glm::vec3(0.6, 0, 1));
    }
    else if(pressedH)
    {
        renderText("H",  x  , y , 0.45, glm::vec3(0.6, 0, 1));
    }
    else if(pressedK)
    {
        renderText("K",  x  , y, 0.45, glm::vec3(0.6, 0, 1));
    }
}

void renderPoint()
{
    GLfloat x, y;
    x = gWidth - (gWidth * 0.99) + 460 + (float)gWidth/450.0;
    y = gHeight - gHeight/30;
    renderText("Points: " +  std::to_string(point), x , y , 0.45, glm::vec3(1, 1, 0));
}

void display()
{
    glClearColor(0, 0, 0, 1);
    glClearDepth(1.0f);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    drawCurrentObject();
    drawGround();
    drawScene();
    renderPoint();
    renderView();
    checkCollission();
    turn90();
    checkLevelCollapse();
    showPressedButton();

    assert(glGetError() == GL_NO_ERROR);
}

void reshape(GLFWwindow* window, int w, int h)
{
    w = w < 1 ? 1 : w;
    h = h < 1 ? 1 : h;

    gWidth = w;
    gHeight = h;

    glViewport(0, 0, w, h);

	// Use perspective projection

	float fovyRad = (float) (45.0 / 180.0) * M_PI;
	projectionMatrix = glm::perspective(fovyRad, gWidth / (float) gHeight, 1.0f, 100.0f);

    // always look toward (0, 0, 0)
	viewingMatrix = glm::lookAt(eyePos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

    for (int i = 0; i < 3; ++i)
    {
        glUseProgram(gProgram[i]);
        glUniformMatrix4fv(projectionMatrixLoc[i], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        glUniformMatrix4fv(viewingMatrixLoc[i], 1, GL_FALSE, glm::value_ptr(viewingMatrix));
    }
}

bool checkHorizontalCollision(int x, int z)
{
    for(int i = -1; i < 2 ; i++)
    {
        for(int j = -1; j < 2 ; j++ )
        {
            for(int k = 0; k < 3 ; k++)
            {
                if(grid[  x + 4 + i ][ (int)curLevel + 4 + k][ z + 4  + j]) return true;
            }
        }
    }
    return false;
}

void checkBorders(int key)
{
    if(curView == FRONT)
    {
        if(key ==  GLFW_KEY_A)
        {
            if( curObjX == -3  || checkHorizontalCollision(curObjX-1, curObjZ) ) return;
            curObjX --;

        }
        else if( key == GLFW_KEY_D){
            if( curObjX == 3 || checkHorizontalCollision(curObjX+1, curObjZ) )   return;
            curObjX++;
        }

    }
    else if(curView == BACK)
    {
        if(key ==  GLFW_KEY_A)
        {
            if( curObjX == 3  || checkHorizontalCollision(curObjX+1, curObjZ) ) return;
            curObjX ++;

        }
        else if( key == GLFW_KEY_D){
            if( curObjX == -3 || checkHorizontalCollision(curObjX-1, curObjZ) )   return;
            curObjX--;
        }
    }
    else if(curView == RIGHT)
    {
        if(key ==  GLFW_KEY_A)
        {
            if( curObjZ == 3  || checkHorizontalCollision(curObjX, curObjZ + 1) ) return;
            curObjZ++;

        }
        else if( key == GLFW_KEY_D){
            if( curObjZ == -3 || checkHorizontalCollision(curObjX, curObjZ - 1) )   return;
            curObjZ--;
        }
    }
    else if(curView == LEFT)
    {
        if(key ==  GLFW_KEY_A)
        {
            if( curObjZ == -3  || checkHorizontalCollision(curObjX, curObjZ - 1) ) return;
            curObjZ--;

        }
        else if( key == GLFW_KEY_D){
            if( curObjZ == 3 || checkHorizontalCollision(curObjX, curObjZ + 1) )   return;
            curObjZ++;
        }
    }
}

void decreaseAngle()
{
    angleLimit -= 90.0;
    decreaseAngleVar = true;
}

void increaseAngle()
{
    angleLimit += 90.0;
    increaseAngleVar = true;
}

void setButtonState(int key, int action)
{
    if(key == GLFW_KEY_A)
    {
        if( action == GLFW_PRESS ) pressedA = true;
        else pressedA = false;
    }
    if(key == GLFW_KEY_S)
    {
        if( action == GLFW_PRESS ) pressedS = true;
        else pressedS = false;
    }
    if(key == GLFW_KEY_D)
    {
        if( action == GLFW_PRESS ) pressedD = true;
        else pressedD = false;
    }
    if(key == GLFW_KEY_W)
    {
        if( action == GLFW_PRESS ) pressedW = true;
        else pressedW = false;
    }
    if(key == GLFW_KEY_H)
    {
        if( action == GLFW_PRESS ) pressedH = true;
        else pressedH = false;
    }
    if(key == GLFW_KEY_K)
    {
        if( action == GLFW_PRESS ) pressedK = true;
        else pressedK = false;
    }
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{

    setButtonState(key, action);

    if ((key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if( key == GLFW_KEY_S  && action == GLFW_PRESS)
    {
        if(intervalForFalling == 0.0) // the game has not started yet
            intervalForFalling = 1.5f;
        else if(intervalForFalling < 0.1) // do not speed again, we have reachted the limit
            return;
        else intervalForFalling /= 3; // divide by 3 the interval to make it faster
    }

    if( key == GLFW_KEY_W  && action == GLFW_PRESS)
    {
        if(intervalForFalling > 1.5) intervalForFalling = 0.0f;
        else intervalForFalling *= 3; // triple the interval to slow down
    }

    if( key == GLFW_KEY_K  && action == GLFW_PRESS) // turn each object 90 degree  or turn eye -90 degree
    {
        if(increaseAngleVar || decreaseAngleVar) return; // DO NOT USER ALLOW TO ROTATE AGAIN WHILE CAMERA IS ROTATING
        increaseAngle();
        curView = (curView + 1) % 4;
    }

    if( key == GLFW_KEY_H  && action == GLFW_PRESS) // turn each object 90 degree  or turn eye -90 degree
    {
        if(increaseAngleVar || decreaseAngleVar) return; // DO NOT USER ALLOW TO ROTATE AGAIN WHILE CAMERA IS ROTATING
        decreaseAngle();
        curView = (curView + 3 ) % 4;
    }
    

    if( ( key == GLFW_KEY_A || key == GLFW_KEY_D ) && action == GLFW_PRESS )
    {   
        if(increaseAngleVar || decreaseAngleVar) return; // DO NOT USER ALLOW TO MOVE OBJECTS WHILE CAMERA IS ROTATING
        checkBorders(key);
    }
}

void mainLoop(GLFWwindow* window)
{
    while (!glfwWindowShouldClose(window))
    {
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

int main(int argc, char** argv)   // Create Main Function For Bringing It All Together
{
    GLFWwindow* window;
    if (!glfwInit())
    {
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(gWidth, gHeight, "tetrisGL", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Initialize GLEW to setup the OpenGL Function pointers
    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    char rendererInfo[512] = {0};
    strcpy(rendererInfo, (const char*) glGetString(GL_RENDERER));
    strcat(rendererInfo, " - ");
    strcat(rendererInfo, (const char*) glGetString(GL_VERSION));
    glfwSetWindowTitle(window, rendererInfo);

    init();

    glfwSetKeyCallback(window, keyboard);
    glfwSetWindowSizeCallback(window, reshape);

    reshape(window, gWidth, gHeight); // need to call this once ourselves
    mainLoop(window); // this does not return unless the window is closed

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
