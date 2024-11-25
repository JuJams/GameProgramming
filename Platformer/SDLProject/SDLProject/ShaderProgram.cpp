#define GL_SILENCE_DEPRECATION

#include "ShaderProgram.h"

void ShaderProgram::Load(const char *vertexShaderFile, const char *fragmentShaderFile) {
    
    
    vertexShader = LoadShaderFromFile(vertexShaderFile, GL_VERTEX_SHADER);
    
    fragmentShader = LoadShaderFromFile(fragmentShaderFile, GL_FRAGMENT_SHADER);
    

    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);
    
    GLint linkSuccess;
    glGetProgramiv(programID, GL_LINK_STATUS, &linkSuccess);
    if(linkSuccess == GL_FALSE) {
	printf("Error linking shader program!\n");
    }
    
    modelMatrixUniform = glGetUniformLocation(programID, "modelMatrix");
    projectionMatrixUniform = glGetUniformLocation(programID, "projectionMatrix");
    viewMatrixUniform = glGetUniformLocation(programID, "viewMatrix");
	colorUniform = glGetUniformLocation(programID, "color");
    
    positionAttribute = glGetAttribLocation(programID, "position");
    texCoordAttribute = glGetAttribLocation(programID, "texCoord");
	
	SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    
}

void ShaderProgram::Cleanup() {
    glDeleteProgram(programID);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

GLuint ShaderProgram::LoadShaderFromFile(const std::string &shaderFile, GLenum type) {
    
    std::ifstream infile(shaderFile);
    
    if(infile.fail()) {
        std::cout << "Error opening shader file:" << shaderFile << std::endl;
    }
    
    
    std::stringstream buffer;
    buffer << infile.rdbuf();
    
    
    return LoadShaderFromString(buffer.str(), type);
}

GLuint ShaderProgram::LoadShaderFromString(const std::string &shaderContents, GLenum type) {
    
    
    
    GLuint shaderID = glCreateShader(type);
    
        
    const char *shaderString = shaderContents.c_str();
    GLint shaderStringLength = (GLint) shaderContents.size();
    
    
    glShaderSource(shaderID, 1, &shaderString, &shaderStringLength);
    glCompileShader(shaderID);
    
    
    GLint compileSuccess;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileSuccess);
    
    
    if (compileSuccess == GL_FALSE) {
        GLchar messages[512];
        glGetShaderInfoLog(shaderID, sizeof(messages), 0, &messages[0]);
        std::cout << messages << std::endl;
    }
    
    
    return shaderID;
}

void ShaderProgram::SetColor(float r, float g, float b, float a) {
	glUseProgram(programID);
	glUniform4f(colorUniform, r, g, b, a);
}

void ShaderProgram::SetViewMatrix(const glm::mat4 &matrix) {
    glUseProgram(programID);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, &matrix[0][0]);
}

void ShaderProgram::SetModelMatrix(const glm::mat4 &matrix) {
    glUseProgram(programID);
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, &matrix[0][0]);
}

void ShaderProgram::SetProjectionMatrix(const glm::mat4 &matrix) {
    glUseProgram(programID);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, &matrix[0][0]);    
}
