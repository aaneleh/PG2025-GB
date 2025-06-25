#include <iostream>
#include <string>
#include <assert.h>
#include <cmath>
#include <thread>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// STB_IMAGE
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

//GLM
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

#include <fstream>

// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar *vertexShaderSource = R"(
#version 400
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texc;
out vec2 tex_coord;
uniform mat4 model;
uniform mat4 projection;
void main()
{
tex_coord = vec2(texc.s, 1.0 - texc.t);
gl_Position = projection * model * vec4(position, 1.0);
}
)";

// Código fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar *fragmentShaderSource = R"(
#version 400
in vec2 tex_coord;
out vec4 color;
uniform sampler2D tex_buff;
uniform vec2 offsetTex;

void main()
{
	color = texture(tex_buff,tex_coord + offsetTex);
}
)";

struct Player {
	GLuint VAO;
	GLuint texID;
	vec3 position;
	vec3 dimensions; //tamanho do frame
	float ds, dt;
	int iAnimation, iFrame;
	int nAnimations, nFrames;
	bool isWalking = false;
    int tileMapLine = 1;
    int tileMapColumn = 1;
};

Player personagem;

int setupSprite(int nAnimations, int nFrames, float &ds, float &dt);


struct Tile {
    GLuint VAO;
    GLuint texID;
    int iTile;
    vec3 position;
    vec3 dimensions; // tamanho do losango 2:1
    float ds, dt;
};

int setupTile(int nTiles, float &ds, float &dt);

vector<Tile> tileset;

void desenharMapa(GLuint shaderID);

void desenharItens(GLuint shaderID, GLuint itemTexID);

void readMap(string filename);


void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

int setupShader();

int loadTexture(string filePath, int &width, int &height);


const GLuint WIDTH = 1000, HEIGHT = 800;

int TILEMAP_WIDTH = 15;
int TILEMAP_HEIGHT = 15;

int t_width;
int t_height;


// Matriz de itens: 0 = nada, 1 = item, 2 = obstáculo
int itens[5][5] = {
    0, 1, 0, 0, 2,
    0, 0, 0, 1, 0,
    1, 0, 2, 0, 0,
    0, 2, 0, 0, 1,
    0, 0, 0, 0, 0
};

int itens_coletados = 0;
int total_itens = 4;

float tile_inicial_x; // centro do eixo x - o valor da metade da largura para centralizar o tilemap na janela
float tile_inicial_y; // divisão da altura da janela pela quantidade de linhas + metade do valor da altura para centralizar o tilemap também no eixo y

vector<int> map;

int main(){

	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 8);
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Jogo coleta - GrauB", nullptr, nullptr);
	if (!window)	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		return -1;
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	//CARREGANDO SHADER
	GLuint shaderID = setupShader();
	glUseProgram(shaderID);

	readMap("../src/tilemap.txt");
	tile_inicial_x = 400 - t_height; // centro do eixo x - o valor da metade da largura para centralizar o tilemap na janela
	tile_inicial_y = (600 / TILEMAP_HEIGHT) + t_height/2; // divisão da altura da janela pela quantidade de linhas + metade do valor da altura para centralizar o tilemap também no eixo y
	
	std::cout << "X: " << tile_inicial_x << std::endl;
	std::cout << "Y: " << tile_inicial_y << std::endl;
	//CARREGANDO PERSONAGEM
	int imgWidth, imgHeight;
	personagem.nAnimations = 4;
	personagem.nFrames = 10;
	personagem.VAO = setupSprite(personagem.nAnimations,personagem.nFrames,personagem.ds,personagem.dt);
	personagem.position = vec3(0, 0, 1.0);
	personagem.dimensions = vec3(35, 35, 1.0);
	GLuint personagemID = loadTexture("../assets/sprites/sprite_full.png",imgWidth,imgHeight);
	personagem.texID = personagemID;
	personagem.iAnimation = 1;
	personagem.iFrame = 0;
    
	//CARRENGADO TILESET
	


	//ATIVANDO TEXTURA
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shaderID, "tex_buff"), 0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//MATRIZ DE PROJEÇÃO
	mat4 projection = ortho(0.0, 800.0, 0.0, 600.0, -1.0, 1.0);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));


	double lastTime = 0.0;
	double deltaT = 0.0;
	double currTime = glfwGetTime();
	double FPS = 12.0;

	total_itens = 0;
	for (int i = 0; i < TILEMAP_HEIGHT; i++)
		for (int j = 0; j < TILEMAP_WIDTH; j++)
			if (itens[i][j] == 1) total_itens++;

	bool venceu = false;
	
	while (!glfwWindowShouldClose(window))	{
		
		glfwPollEvents();

		glClearColor(0.54f, 0.77f, 0.98f, 1.0f); // cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

        desenharMapa(shaderID);

        mat4 model = mat4(1);
		currTime = glfwGetTime();
		deltaT = currTime - lastTime;
		
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        float x = 0;
        float y = 0;

        if(personagem.tileMapLine > TILEMAP_HEIGHT){
            personagem.tileMapLine = TILEMAP_HEIGHT;
        } 
        if(personagem.tileMapLine < 1){
            personagem.tileMapLine = 1;
        } 
        if(personagem.tileMapColumn > TILEMAP_WIDTH){
            personagem.tileMapColumn = TILEMAP_WIDTH;
        }
        if(personagem.tileMapColumn < 1){
            personagem.tileMapColumn = 1;
        } 

        x = tile_inicial_x + t_height + (personagem.tileMapColumn - personagem.tileMapLine) * t_height;
        y = tile_inicial_y + (personagem.tileMapColumn + personagem.tileMapLine) * (t_height/2);

        personagem.position.x = x;
        personagem.position.y = y;

		// Lógica de coleta
		int lin = 1;
		int col = 1;

		if (lin >= 0 && lin < TILEMAP_HEIGHT && col >= 0 && col < TILEMAP_WIDTH){
			if (map[lin + col * TILEMAP_HEIGHT] == 4) {
				for (int i = 0; i < TILEMAP_HEIGHT; i++)
					for (int j = 0; j < TILEMAP_WIDTH; j++)
						if (map[i + j * TILEMAP_HEIGHT] == 4) map[i + j * TILEMAP_HEIGHT] = 1;
				std::cout << "Todos os tiles 4 foram trocados por 1." << std::endl;
			}
		}

		if (!venceu && (itens_coletados == total_itens || map[lin + col * TILEMAP_HEIGHT] == 9)) {
    		venceu = true;
			if (itens_coletados == total_itens)
				std::cout << "Você venceu! Todos os itens coletados." << std::endl;
			else
				std::cout << "Você venceu ao chegar no tile objetivo!" << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(2));
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		// Desenho do personagem
		model = mat4(1);
		model = translate(model, personagem.position);
		model = rotate(model, radians(0.0f), vec3(0.0, 0.0, 1.0));
		model = scale(model,personagem.dimensions);
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));

		vec2 offsetTex;
		
		if (deltaT >= 1.0/FPS){
			if(personagem.isWalking) {
				personagem.iFrame = (personagem.iFrame + 1) % personagem.nFrames; // incremento "circular"
			}
			lastTime = currTime;
		}
		
		offsetTex.s = personagem.iFrame * personagem.ds;
		offsetTex.t = (personagem.iAnimation) * personagem.dt;
		glUniform2f(glGetUniformLocation(shaderID, "offsetTex"),offsetTex.s, offsetTex.t);

		glBindVertexArray(personagem.VAO);
		glBindTexture(GL_TEXTURE_2D, personagem.texID);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glfwSwapBuffers(window);
	}
		
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode){
	if (action == GLFW_PRESS) {
        std::cout << "[DEBUG] Tecla pressionada: " << key << std::endl;
    }
	personagem.isWalking = true;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (action != GLFW_PRESS && action != GLFW_REPEAT){
		personagem.isWalking = false;
	} 
	
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS){
        personagem.iAnimation = 2;
        personagem.tileMapColumn -= 1;
    }
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
        personagem.iAnimation = 4;
        personagem.tileMapColumn += 1;
    }
    if (key == GLFW_KEY_UP && action == GLFW_PRESS){
        personagem.iAnimation = 3;
        personagem.tileMapLine += 1;
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS){
        personagem.iAnimation = 1;
        personagem.tileMapLine -= 1;
    }	

    if (key == GLFW_KEY_A && action == GLFW_PRESS){
        personagem.iAnimation = 2;
        personagem.tileMapLine += 1;
        personagem.tileMapColumn -= 1;
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS){
        personagem.iAnimation = 4;
        personagem.tileMapLine -= 1;
        personagem.tileMapColumn += 1;
    }
    if (key == GLFW_KEY_W && action == GLFW_PRESS){
        personagem.iAnimation = 3;
        personagem.tileMapLine += 1;
        personagem.tileMapColumn += 1;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS){
        personagem.iAnimation = 1;
        personagem.tileMapLine -= 1;
        personagem.tileMapColumn -= 1;
    }
}

void readMap(string filename) {
	ifstream file(filename);

	//NOME DO ARQUIVO DE TEXTURA
	string tilemapFile;
	file >> tilemapFile;
	string texture = "../assets/tilesets/" + tilemapFile;
    int imgWidth, imgHeight;
    GLuint texID = loadTexture(texture, imgWidth, imgHeight);

	//NUMERO DE TILES NO ARQUIVO
	string num_tiles;
	file >> num_tiles;   

	//DIMENSOES TILE
	string tile_height, tile_width;
	file >> tile_height;    
	file >> tile_width;
	
	t_height = stoi(tile_height);
	t_width = stoi(tile_width);

    for (int i = 0; i < stoi(num_tiles); i++){
        Tile tile;
        tile.dimensions = vec3(stoi(tile_width), stoi(tile_height), 1.0);
        tile.iTile = i;
        tile.texID = texID;
        tile.VAO = setupTile( stoi(num_tiles), tile.ds, tile.dt);
        tileset.push_back(tile);
    }

	//TAMANHO MATRIZ DO MAP
	string tilemap_h, tilemap_w;
	file >> tilemap_h;    
	file >> tilemap_w;   
	TILEMAP_HEIGHT = stoi(tilemap_h);
	TILEMAP_HEIGHT = stoi(tilemap_w);


	//CRIA O TILE MAP
	string type;
	for(int i = 0 ; i < TILEMAP_HEIGHT*TILEMAP_WIDTH; i ++){
		file >> type;
		map.push_back(stoi(type));
	} 

	//ADICIONA TILES 'CAMINHÁVEIS' PRA CONSTANTE 'VALID_TILES'
	/* 
	string currentEl;
	do {
		file >> currentEl;
		if(stoi(currentEl) == -1) break;
		VALID_TILES.push_back(stoi(currentEl));
	} while (true);


	//CRIA OS COLETÁVEIS
	collectible collectibleEl;
	string textureCollectible;
	file >> textureCollectible;
	collectibleEl.texID = loadTexture("../assets/" + textureCollectible, imgWidth, imgHeight);
	collectibleEl.VAO = createVAOPlayer(1, 1, collectibleEl.ds, collectibleEl.dt);

	string posX, posY;
	while(file >> posX){ 
		file >> posY;
		collectibleEl.tileMapLine = stoi(posX);
		collectibleEl.tileMapColumn = stoi(posY);
		collectibles.push_back(collectibleEl);
		//cout << ">>TILELINE: " << collectibleEl.tileMapLine << "\nTILECOLUMN: " << collectibleEl.tileMapColumn << endl;
	}  */

	file.close();
}

int setupSprite(int nAnimations, int nFrames, float &ds, float &dt){

	ds = 1.0 / (float) nFrames;
	dt = 1.0 / (float) nAnimations;
	
	GLfloat vertices[] = {
		// x   y    z    s     t
		-0.5,  0.5, 0.0, 0.0, dt, //V0
		-0.5, -0.5, 0.0, 0.0, 0.0, //V1
		 0.5,  0.5, 0.0, ds, dt, //V2
		 0.5, -0.5, 0.0, ds, 0.0  //V3
	};

	GLuint VBO, VAO;

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}

int setupTile(int nTiles, float &ds, float &dt){

    ds = 1.0 / (float)nTiles;
    dt = 1.0;

    float th = 1.0, tw = 1.0;

    GLfloat vertices[] = {
        // x   y    z    s     t
        0.0, th / 2.0f, 0.0, 0.0, dt / 2.0f, // A
        tw / 2.0f, th, 0.0, ds / 2.0f, dt,   // B
        tw / 2.0f, 0.0, 0.0, ds / 2.0f, 0.0, // D
        tw, th / 2.0f, 0.0, ds, dt / 2.0f    // C
    };

    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    return VAO;
}

void desenharMapa(GLuint shaderID){
    // dá pra fazer um cálculo usando tilemap_width e tilemap_height
    // float x0 = 400;
    // float y0 = 100;

    for (int i = 0; i < TILEMAP_HEIGHT; i++){
        for (int j = 0; j < TILEMAP_WIDTH; j++){
            // Matriz de transformaçao do objeto - Matriz de modelo
            mat4 model = mat4(1); // matriz identidade

            Tile curr_tile = tileset[map[i + j * TILEMAP_HEIGHT]];

            float x = tile_inicial_x + (j - i) * curr_tile.dimensions.x/2.0;
            float y = tile_inicial_y + (i + j) * curr_tile.dimensions.y/2.0;
        
            model = translate(model, vec3(x, y, 0.0));
            model = scale(model, curr_tile.dimensions);
            glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));

            vec2 offsetTex;

            offsetTex.s = curr_tile.iTile * curr_tile.ds;
            offsetTex.t = 0.0;
            glUniform2f(glGetUniformLocation(shaderID, "offsetTex"), offsetTex.s, offsetTex.t);

            glBindVertexArray(curr_tile.VAO);              
            glBindTexture(GL_TEXTURE_2D, curr_tile.texID); 

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    }
}

int loadTexture(string filePath, int &width, int &height){
	GLuint texID;

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int nrChannels;

	unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

	if (data){
		if (nrChannels == 3){
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}else{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

int setupShader(){
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
	}

    GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
				  << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}