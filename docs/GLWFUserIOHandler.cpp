Source code : C++ from... main.cpp
#include <glad/glad.h> // GLAD: https://github.com/Dav1dde/glad ... GLAD 2 also works via the web-service: https://gen.glad.sh/ (leaving all checkbox options unchecked)
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// OpenGL Mathematics(GLM) ... https://github.com/g-truc/glm/blob/master/manual.md
// ------------------------------------
// GLM Headers
// ------------------
#include <glm/glm.hpp> // Include all GLM core.
	// #include <glm/ext.hpp> // Include all GLM extensions.
#include <glm/gtc/matrix_transform.hpp> // Specific extensions.
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <iostream>
#include <fstream> // Used in "shader_configure.h" to read the shader text files.

#include "shader_configure.h" // Used to create the shaders.
#include "load_meshes_binary.h"

#include "window_params.h"
#include "player_control.h"
#include "shadow_maps.h"
#include "callback_functions.h"

	void show_FPS(std::vector<Model>& model_list, double& time, int& skip); // Function prototype.

int main()
{
	// (1) Create window
	// ------------------------
	Window_Params window_params; // Only use parentheses if the constructor has parameters.

	// (2) Compile Shaders Read from Text Files
	// ------------------------------------------------------
	const char* vert_shader = "../../Shaders/shader_glsl.vert";
	const char* frag_shader = "../../Shaders/shader_glsl.frag";

	Shader main_shader(vert_shader, frag_shader);
	main_shader.use();

	// (3) Initiate the Player
	// ----------------------------
	Player player(window_params.init_window_width, window_params.init_window_height, main_shader);

	// (4) Set Each Light's Camera Parameters
	// ----------------------------------------------------	
	glm::vec3 persp_light_pos(-12.0f, 12.0f, -22.0f);
	glm::vec3 persp_light_target(0.0f, 0.0f, 0.0f);
	float shadow_cam_FOV = 90.0f;
	Shadow light_source_0(main_shader, persp_light_pos, persp_light_target, shadow_cam_FOV); // Perspective.

	glm::vec3 orth_light_pos(5.0f, 12.0f, -19.0f);
	glm::vec3 orth_light_target(0.0f, 0.0f, 0.0f);
	float left = -15.0f;
	float right = 15.0f;
	float bottom = -15.0f;
	float top = 15.0f;
	Shadow light_source_1(main_shader, orth_light_pos, orth_light_target, left, right, bottom, top); // Orthographic.

	unsigned int light_ID_loc = glGetUniformLocation(main_shader.ID, "light_ID");
	unsigned int light_count_loc = glGetUniformLocation(main_shader.ID, "light_count");
	unsigned int rendering_shadow_map_loc = glGetUniformLocation(main_shader.ID, "rendering_shadow_map");

	// (5) Set GLFW Callback Function Pointers
	// -----------------------------------------------------
	Callback_Functions callback_state(player, light_source_0, window_params);
	glfwSetWindowUserPointer(window_params.window, &callback_state);
	glfwSetJoystickUserPointer(GLFW_JOYSTICK_1, &callback_state);

	// (6) Load Blender Models
	// --------------------------------
	// https://www.turbosquid.com/Search/3D-Models/free/commercial (Free Models)
	// Note: remember to delete at least one of a particular model's files within the relevant model folder after modifying it within Blender (Watch: Model Loading Tutorial Part 3)

	// Model black_smith("Object & Material Files\\black_smith.obj", main_shader, 0); // Draw method: 0 = multiple meshes... 1 = combined (5 VBOs) ... 2 = combined (1 VBO)
	// Model helicopter("Object & Material Files\\The_Beast_Helicopter.obj", main_shader, 1);
	Model aeroplane("Object & Material Files\\Plane_CAP_232.obj", main_shader, 0);
	Model flat_plane("Object & Material Files\\flat_plane_with_image.obj", main_shader, 1);
	Model light_0("Object & Material Files\\light.obj", main_shader, 2);
	Model light_1("Object & Material Files\\light.obj", main_shader, 0);
	Model frustum("Object & Material Files\\frustum.obj", main_shader, 1);

	std::vector<Model> model_list; // Used in: show_FPS() further down.
	model_list.push_back(aeroplane);
	model_list.push_back(flat_plane);
	model_list.push_back(light_0);
	model_list.push_back(light_1);
	model_list.push_back(frustum);

	unsigned int model_number_loc = glGetUniformLocation(main_shader.ID, "model_number");
	unsigned int spin_on_the_spot_mat_loc = glGetUniformLocation(main_shader.ID, "spin_on_the_spot_mat");

	// (7) Enter the Main-Loop
	// --------------------------------
	srand((unsigned)time(NULL)); // Initialise random seed.

	float x_spin = 1.0f / (rand() % 10 + 1); // Generate random number between 1 and 10
	float y_spin = 1.0f / (rand() % 10 + 1);
	float z_spin = 1.0f / (rand() % 10 + 1);
	float spin_speed = (float)(rand() % 5 + 1); // Cast is simply to silence the compiler warning.

	float spin_vary = 0.0f;
	int spin_dir = 1;

	glm::mat4 spinning_mat(1.0f);

	int skip = 0; // Used in: show_FPS() further down.
	double time = 0;

	while (!glfwWindowShouldClose(window_params.window)) // Enter main-Loop.
	{
		// (8) Player & Lights Control
		// -----------------------------------
		player.yaw_tilt_keys_val(window_params.window, 0.75f); // Comment to disable.
		// player.continuously_rotate_view(); // 1) Currently gets reset when moving the mouse... 2) Gets completely overridden if the line above is not commented.

		// light_source_0.rotate_transformation_matrix();
		light_source_0.transform_light_position();

		light_source_1.rotate_transformation_matrix();
		light_source_1.transform_light_position();

		// (9) Randomise the Model's Spinning Speed & Axis
		// ------------------------------------------------------------------
		spin_vary += 0.05f * spin_dir;

		if (spin_vary > 6 || spin_vary < 0)
		{
			spin_dir = -spin_dir; // Reverse the spinning direction.

			x_spin = 1.0f / (rand() % 10 + 1);
			y_spin = 1.0f / (rand() % 10 + 1);
			z_spin = 1.0f / (rand() % 10 + 1);
			spin_speed = (float)(rand() % 50 + 1) / 20;
		}
		spinning_mat = glm::rotate(spinning_mat, glm::radians(spin_speed), glm::normalize(glm::vec3(x_spin, y_spin, z_spin)));
		glUniformMatrix4fv(spin_on_the_spot_mat_loc, 1, GL_FALSE, glm::value_ptr(spinning_mat)); // Pass rotation matrix to vertex shader.		

		// (10) Render Shadow Maps 1st... Draw Models 2nd
		// ------------------------------------------------------------------
		// Render shadows (GLSL: 2.2.1 "When the type of internal state is boolean, zero integer or floating-point values are converted to FALSE and non-zero values are converted to TRUE")
		glUniform1i(rendering_shadow_map_loc, true);

		glUniform1ui(light_ID_loc, 0); // Send light ID to vertex shader whenever 1) Changing light source which is used for shadow rendering... or 2) Drawing the light or frustum model.
		glUniform1ui(model_number_loc, 0);
		light_source_0.render_model_shadow(aeroplane, true);

		glUniform1ui(model_number_loc, 1);
		light_source_0.render_model_shadow(flat_plane, false);

		glUniform1ui(light_ID_loc, 1);
		glUniform1ui(model_number_loc, 0);
		light_source_1.render_model_shadow(aeroplane, true);

		glUniform1ui(model_number_loc, 1);
		light_source_1.render_model_shadow(flat_plane, false);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUniform1i(rendering_shadow_map_loc, false); // Render models.

		glUniform1ui(light_count_loc, 2); // Let both shaders know how many lights there are. This could be added to the: shadow_maps.h class.

		glUniform1ui(model_number_loc, 0); // Likewise this could be added to the: load_meshes_binary.h class.
		aeroplane.process_draw_calls(window_params.frame_buffer_width, window_params.frame_buffer_height);

		glUniform1ui(model_number_loc, 1);
		flat_plane.process_draw_calls(window_params.frame_buffer_width, window_params.frame_buffer_height);

		glUniform1ui(light_ID_loc, 0);
		glUniform1ui(model_number_loc, 2);
		light_0.process_draw_calls(window_params.frame_buffer_width, window_params.frame_buffer_height);

		glUniform1ui(model_number_loc, 4);
		frustum.process_draw_calls(window_params.frame_buffer_width, window_params.frame_buffer_height);

		glUniform1ui(light_ID_loc, 1);
		glUniform1ui(model_number_loc, 3);
		light_1.process_draw_calls(window_params.frame_buffer_width, window_params.frame_buffer_height);

		glUniform1ui(model_number_loc, 4);
		frustum.process_draw_calls(window_params.frame_buffer_width, window_params.frame_buffer_height);

		glfwSwapBuffers(window_params.window);
		glfwPollEvents();

		// show_FPS(model_list, time, skip);
	}
	// (11) Exit the Application
	// --------------------------------
	glDeleteProgram(main_shader.ID); // This OpenGL function call is talked about in: shader_configure.h

	// glfwDestroyWindow(window) // Call this function to destroy a specific window.
	glfwTerminate(); // Destroys all remaining windows and cursors, restores modified gamma ramps, and frees resources.

	exit(EXIT_SUCCESS); // Function call: exit() is a C/C++ function that performs various tasks to help clean up resources.
}

void show_FPS(std::vector<Model>& model_list, double& time, int& skip)
{
	double prev_time = time;
	time = glfwGetTime();

	float delta_time = (float)(time - prev_time);

	++skip; // Avoids std::cout slowing down the program too much. 
	if (skip == 60) // 60 Is simply so that for VSync 60FPS it displays here once per second (Any number is OK)
	{
		skip = 0;
		for (unsigned int i = 0; i < model_list.size(); ++i)
		{
			if (model_list[i].draw_method == 0)
				std::cout << "   Draw method: Multiple Meshes --- Draw calls: " << model_list[i].num_meshes << " --- Model name : " << model_list[i].model_name << "\n";
			if (model_list[i].draw_method == 1)
				std::cout << "   Draw method: 5VBO --- Draw calls: 1 --- Model name : " << model_list[i].model_name << "\n";
			if (model_list[i].draw_method == 2)
				std::cout << "   Draw method: 1VBO --- Draw calls: 1 --- Model name : " << model_list[i].model_name << "\n";
		}
		std::cout << "   FPS: " << ((1.0f / 60) / delta_time) * 60 << "\n\n";
	}
}
Source code : C++ from... shader_configure.h
#pragma once // Instead of using include guards.

class Shader
{
public:
	GLuint ID; // Public Program ID.

	// Constructor
	// ---------------
	Shader(const char* vert_path, const char* frag_path)
	{
		char character;

		std::ifstream vert_stream;
		std::ifstream frag_stream;

		std::string vert_string;
		std::string frag_string;

		// Read vertex shader text file
		// ------------------------------------
		vert_stream.open(vert_path); // I decided not to implement: Exception handling try catch method.

		if (vert_stream.is_open()) // Note: There are various other methods for accessing the stream, i.e., vert_stream.get() is just one option.
		{
			while (vert_stream.get(character)) // Loop getting single characters until EOF (value false) is returned. 
				vert_string += character; // "The first signature returns the character read, or the end-of-file value (EOF) if no characters are available in the stream..."

			vert_stream.close();
			std::cout << "\n   File: " << vert_path << " opened successfully.\n";
		}
		else
			std::cout << "\n   ERROR!... File: " << vert_path << " could not be opened.\n";

		// Read fragment shader text file
		// ----------------------------------------
		frag_stream.open(frag_path);

		if (frag_stream.is_open())
		{
			while (frag_stream.get(character))
				frag_string += character;

			frag_stream.close();
			std::cout << "   File: " << frag_path << " opened successfully.\n\n";
		}
		else
			std::cout << "   ERROR!... File: " << frag_path << " could not be opened.\n\n";

		// std::cout << vert_string << "\n\n"; // Output the shader files to display in the console window.
		// std::cout << frag_string << "\n\n";

		const char* vert_pointer = vert_string.c_str();
		const char* frag_pointer = frag_string.c_str();

		// Compile shaders
		// ----------------------
		GLuint vert_shad, frag_shad; // Declare in here locally. Being attached to the public Program ID allows the shaders to be used publicly.

		// Create vertex shader
		// ---------------------------
		vert_shad = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vert_shad, 1, &vert_pointer, NULL);
		glCompileShader(vert_shad);
		check_shaders_program(vert_shad, "vert_shader");

		// Create fragment shader
		// -------------------------------
		frag_shad = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(frag_shad, 1, &frag_pointer, NULL);
		glCompileShader(frag_shad);
		check_shaders_program(frag_shad, "frag_shader");

		// Create shader program
		// ------------------------------
		ID = glCreateProgram();
		glAttachShader(ID, vert_shad); // This also avoids deletion via: glDeleteShader(vert_shad) as called below.
		glAttachShader(ID, frag_shad);
		glLinkProgram(ID);
		check_shaders_program(ID, "shader_program");

		// Note: Flagging the program object for deletion before calling "glUseProgram" would accidentally stop the program installation of the rendering state	
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		glDeleteShader(vert_shad); // Flag shader object for automatic deletion (freeing memory) when no longer attached to a program object...
		glDeleteShader(frag_shad); // ... program object is deleted (glDeleteProgram ) within: main() when the application ends.

		// glUseProgram(ID); // Typically this is called within: main() to select individual shaders that have been created. 
		// glDeleteProgram(ID); // Alternatively the program object can be deleted here after 1st calling:  glUseProgram(ID)
	}

	// Activate the shader
	// -------------------------
	void use()
	{
		glUseProgram(ID); // Function called from within main() to select an individual shader to be used.
	}

private:
	// Check shader compilations and program object for linking errors
	// -------------------------------------------------------------------------------------
	void check_shaders_program(GLuint type, std::string name)
	{
		int success;
		int error_log_size;
		char info_log[1000]; // 1000 characters max. Typically it's less than 500 even for multiple shader errors.

		if (name == "vert_shader" || name == "frag_shader")
		{
			glGetShaderiv(type, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(type, 1024, &error_log_size, info_log);
				std::cout << "\n--- Shader Compilation Error: " << name << "\n\n" << info_log << "\n" << "Error Log Number of Characters: " << error_log_size << "\n\n";
			}
		}
		else // "shader_program"
		{
			glGetProgramiv(type, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(type, 1024, &error_log_size, info_log);
				std::cout << "\n--- Program Link Error: " << name << "\n\n" << info_log << "\n" << "Error Log Number of Characters: " << error_log_size << "\n";
			}
		}
	}
};
Source code : C++ from... load_meshes_binary.h
#pragma once // Instead of using include guards.

Assimp::Importer importer; // https://assimp-docs.readthedocs.io/en/v5.1.0/ ... (An older Assimp website: http://assimp.sourceforge.net/lib_html/index.html)
const aiScene* scene = nullptr;
aiNode* root_node = nullptr; // Only being used in the: load_model_cout_console() function.

class Model
{
private:
	struct Mesh
	{
		unsigned int VAO, VBO1, VBO2, VBO3, EBO; // Buffer handles (Typically type: GLuint is used)

		std::vector<glm::vec3> vert_positions;
		std::vector<glm::vec3> vert_normals;
		std::vector<glm::vec2> tex_coords;
		std::vector<unsigned int> vert_indices;
		unsigned int mesh_num; // Add a uniform if wanting to use mesh_num via: draw_multiple_meshes()  
		unsigned int tex_handle;
	};
	struct Texture
	{
		unsigned int tex_handle;
		unsigned int sampler_location;
		std::string image_name;
	};
	// ------------------------------
	struct Meshes_5VBO_Combined
	{
		unsigned int VAO, VBO1, VBO2, VBO3, VBO4, VBO5, EBO;
		std::vector<Texture> texture_list;

		std::vector<glm::vec3> vert_positions;
		std::vector<glm::vec3> vert_normals;
		std::vector<glm::vec2> tex_coords;
		std::vector<unsigned int> vert_indices;
		std::vector<unsigned int> mesh_num;
		std::vector<unsigned int> sampler_array_pos;
	};
	// ------------------------------
	struct Vertex
	{
		glm::vec3 vert_position;
		glm::vec3 vert_normal;
		glm::vec2 tex_coords;
		unsigned int mesh_num;
		unsigned int sampler_array_pos;
	};
	struct Meshes_1VBO_Combined
	{
		unsigned int VAO, VBO, EBO;
		std::vector<Texture> texture_list;

		std::vector<unsigned int> vert_indices;
		std::vector<Vertex> vertex_data;
	};
	// ------------------------------	
	bool binary_version_found;

	std::vector<Texture> texture_list;
	std::vector<Mesh> mesh_list;

	Meshes_1VBO_Combined meshes_1VBO_combined;
	Meshes_5VBO_Combined meshes_5VBO_combined;

	Shader& shader;
	unsigned int draw_multiple_meshes_sampler_pos0_val;
	unsigned int rendering_multiple_meshes_loc; // Uniform required because multiple meshes (draw method 0) doesn't pass mesh number via shader input attribute.
	unsigned int meshes_combined_loc;

public:
	std::string model_name; // Public because used in: show_FPS() within main.cpp
	unsigned int num_meshes;
	unsigned int draw_method;

	Model(const char* model_path, Shader& main_shader, unsigned draw_method) : shader(main_shader) // Constructor
	{
		model_name = model_path;
		this->draw_method = draw_method;
		// ---------------------------------------------
		configure_draw_calls(model_path);
	}

	void process_draw_calls(unsigned buffer_width, unsigned buffer_height)
	{
		if (draw_method == 0)
			draw_multiple_meshes(buffer_width, buffer_height);
		if (draw_method == 1)
			draw_meshes_combined_5VBO(buffer_width, buffer_height);
		if (draw_method == 2)
			draw_meshes_combined_1VBO(buffer_width, buffer_height);
	}

private:
	void configure_draw_calls(std::string model_path)
	{
		rendering_multiple_meshes_loc = glGetUniformLocation(shader.ID, "rendering_multiple_meshes");
		meshes_combined_loc = glGetUniformLocation(shader.ID, "meshes_combined");

		if (draw_method == 0)
		{
			load_model(model_path); // Uncomment only one of these two load model functions.
				// load_model_cout_console(model_path);

			draw_multiple_meshes_sampler_pos0_val = glGetUniformLocation(shader.ID, "images[0]");
		}

		if (draw_method == 1)
		{
			binary_version_found = look_for_model_5VBO(model_path);

			if (binary_version_found)
			{
				std::cout << "\n   Binary model found (5VBO) Path: " << model_path;
				std::cout << "\n   ********************************************************\n\n";

				// Measure Loading Time (5VBO)
				// ----------------------------------------
				double time_before_binary = glfwGetTime();
				read_model_files_5VBO(model_path);
				std::cout << "   Time taken to read binary files (5VBO): " << glfwGetTime() - time_before_binary << "\n\n";

				double time_before_textures = glfwGetTime();
				load_file_name_images(meshes_5VBO_combined.texture_list);
				std::cout << "\n   Time taken to load textures (5VBO): " << glfwGetTime() - time_before_textures << "\n\n";

				populate_sampler_array_setup(meshes_5VBO_combined.texture_list); // Includes calls to: glGetUniformLocation()
			}
			else
			{
				std::cout << "\n   Did not find usable binary model (5VBO) Path: " << model_path;
				std::cout << "\n   **********************************************************************\n\n";

				load_model(model_path);	// Load model via Assimp.
					// load_model_cout_console(model_path);

				combine_meshes_5VBO(); // Vertex data as... vector lists.
				write_model_files_5VBO(model_path);

				populate_sampler_array_setup(texture_list);
			}
			set_buffer_data_combined_5VBO(); // Set up: VAO, VBO and EBO.			
		}

		if (draw_method == 2)
		{
			binary_version_found = look_for_model_1VBO(model_path);

			if (binary_version_found)
			{
				std::cout << "\n   Binary model found (1VBO) Path: " << model_path;
				std::cout << "\n   ********************************************************\n\n";

				// Measure Loading Time (1VBO)
				// ----------------------------------------
				double time_before_binary = glfwGetTime();
				read_model_files_1VBO(model_path);
				std::cout << "   Time taken to read binary files (1VBO): " << glfwGetTime() - time_before_binary << "\n\n";

				double time_before_textures = glfwGetTime();
				load_file_name_images(meshes_1VBO_combined.texture_list);
				std::cout << "\n   Time taken to load textures (1VBO): " << glfwGetTime() - time_before_textures << "\n\n";

				populate_sampler_array_setup(meshes_1VBO_combined.texture_list); // Includes calls to: glGetUniformLocation()
			}
			else
			{
				std::cout << "\n   Did not find usable binary model (1VBO) Path: " << model_path;
				std::cout << "\n   **********************************************************************\n\n";

				load_model(model_path); // Load model via Assimp.
					// load_model_cout_console(model_path);

				combine_meshes_1VBO(); // Vertex data as... struct lists.
				write_model_files_1VBO(model_path);

				populate_sampler_array_setup(texture_list);
			}
			set_buffer_data_combined_1VBO(); // Set up: VAO, VBO and EBO.			
		}
	}

	void populate_sampler_array_setup(std::vector<Texture>& texture_list)
	{
		for (unsigned int i = 0; i < texture_list.size(); ++i)
		{
			std::string index_num = std::to_string(i);
			std::string sampler_name = "images[" + index_num + "]";

			// Sampler location set below is received here via texture list (by reference)
			// ------------------------------------------------------------------------------------------------
			texture_list[i].sampler_location = glGetUniformLocation(shader.ID, sampler_name.c_str());
		}
	}

	void load_model(std::string model_path)
	{
		// http://assimp.sourceforge.net/lib_html/postprocess_8h.html (See: aiPostProcessSteps) (Flag options)			
		scene = importer.ReadFile(model_path, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
			std::cout << "Assimp importer.ReadFile (Error) -- " << importer.GetErrorString() << "\n";
		else
		{
			num_meshes = scene->mNumMeshes;
			mesh_list.resize(num_meshes);

			aiMesh* mesh{};
			unsigned int total_num_vertices = 0;
			unsigned int total_num_indices = 0;

			// (1) Loop through all the model's meshes
			// -----------------------------------------------------
			for (unsigned int i = 0; i < num_meshes; ++i)
			{
				mesh_list[i].mesh_num = i; // Fed in to vertex shader as an input attribute (Used to identify and transform meshes independently of one another)
				mesh = scene->mMeshes[i]; // http://assimp.sourceforge.net/lib_html/structai_mesh.html
				std::cout << "   mesh_list[i].mesh_num: " << mesh_list[i].mesh_num << "\n";

				total_num_vertices += mesh->mNumVertices;
				// std::cout << "   Mesh: " << i << " --- mesh[i].mNumVertices: " << mesh->mNumVertices << "\n";

				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex]; // http://assimp.sourceforge.net/lib_html/structai_material.html	

				unsigned int tex_count = 0; // This loop will only run once (i.e. there's only 1 texture per mesh)
				for (; tex_count < material->GetTextureCount(aiTextureType_DIFFUSE); ++tex_count) // Also, only using: aiTextureType_DIFFUSE.
				{
					aiString string;
					material->GetTexture(aiTextureType_DIFFUSE, tex_count, &string);	 // Acquire the name of the image file to be loaded.			

					// (2) Load mesh [i]'s texture if not already loaded
					// ---------------------------------------------------------------
					int already_loaded = is_image_loaded(string.C_Str()); // Returns -1 if texture Not already loaded, otherwise returns Existing texture handle.

					if (already_loaded == -1) // Image not yet loaded so now attempt to load it.
					{
						bool load_success = false;
						unsigned int texture_handle = load_texture_image(string.C_Str(), load_success); // Notice the UPPER case "C_Str()"

						if (load_success) // Although do nothing if the image fails to load.
						{
							Texture texture;
							texture.image_name = string.C_Str();
							texture.tex_handle = texture_handle;
							texture_list.push_back(texture);

							mesh_list[i].tex_handle = texture_handle;
						}
						std::cout << "\n";
					}
					else
						mesh_list[i].tex_handle = already_loaded; // Assign existing texture handle.
				}
				if (tex_count == 0)
				{
					mesh_list[i].tex_handle = 0;
					// std::cout << "   material->GetTexture(...) No image has been applied to this mesh\n";
				}

				// (3) Loop through all mesh [i]'s vertices
				// ---------------------------------------------------
				for (unsigned int i2 = 0; i2 < mesh->mNumVertices; ++i2)
				{
					glm::vec3 position{};
					position.x = mesh->mVertices[i2].x;
					position.y = mesh->mVertices[i2].y;
					position.z = mesh->mVertices[i2].z;
					mesh_list[i].vert_positions.push_back(position);

					if (mesh->HasNormals())
					{
						glm::vec3 normal{};
						normal.x = mesh->mNormals[i2].x;
						normal.y = mesh->mNormals[i2].y;
						normal.z = mesh->mNormals[i2].z;
						mesh_list[i].vert_normals.push_back(normal);
					}
					else
						mesh_list[i].vert_normals.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

					if (mesh->HasTextureCoords(0)) // Only slot [0] is in question.
					{
						glm::vec2 tex_coords{};
						tex_coords.x = mesh->mTextureCoords[0][i2].x;
						tex_coords.y = mesh->mTextureCoords[0][i2].y;
						mesh_list[i].tex_coords.push_back(tex_coords);
					}
					else
						mesh_list[i].tex_coords.push_back(glm::vec2(0.0f, 0.0f));
				}
				// (4) Loop through all mesh [i]'s Indices
				// --------------------------------------------------
				for (unsigned int i3 = 0; i3 < mesh->mNumFaces; ++i3)
				{
					for (unsigned int i4 = 0; i4 < mesh->mFaces[i3].mNumIndices; ++i4)
						mesh_list[i].vert_indices.push_back(mesh->mFaces[i3].mIndices[i4]);
					total_num_indices += mesh->mFaces[i3].mNumIndices;
				}
				set_buffer_data(i); // Set up: VAO, VBO and EBO.
			}
			std::cout << "\n   Assimp... total_num_vertices: " << total_num_vertices << "\n";
			std::cout << "   Assimp... total_num_indices: " << total_num_indices << "\n\n";
		}
	}

	void load_model_cout_console(std::string model_path)
	{
		// http://assimp.sourceforge.net/lib_html/postprocess_8h.html (See: aiPostProcessSteps) (Flag options)			
		scene = importer.ReadFile(model_path, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_FlipUVs);

		// Briefly looking at the node structure
		// ------------------------------------------------
		if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
			std::cout << "Assimp importer.ReadFile (Error) -- " << importer.GetErrorString() << "\n";
		else
		{
			num_meshes = scene->mNumMeshes;
			mesh_list.resize(num_meshes);

			std::cout << "\n   Start of Assimp Loading Meshes & Analysis";
			std::cout << "\n   -----------------------------------------";

			root_node = scene->mRootNode;

			std::cout << "\n   node->mNumMeshes: " << root_node->mNumMeshes;
			std::cout << "\n   node->mName.C_Str(): " << root_node->mName.C_Str();
			std::cout << "\n\n   node->mNumChildren: " << root_node->mNumChildren;
			// ------------------------------------------------------------------------------------------
			for (unsigned int i = 0; i < root_node->mNumChildren; ++i)
			{
				std::cout << "\n   node->mChildren[i]->mName.C_Str(): " << root_node->mChildren[i]->mName.C_Str();
				std::cout << "\n   node->mChildren[i]->mNumMeshes: " << root_node->mChildren[i]->mNumMeshes;
			}
			std::cout << "\n\n   scene->HasMaterials(): " << scene->HasMaterials();
			// ------------------------------------------------------------------------------------------
			for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
				std::cout << "\n   scene->mMaterials[i]->GetName(): " << scene->mMaterials[i]->GetName().C_Str();

			std::cout << "\n\n   scene->HasTextures(): " << scene->HasTextures();

			aiMesh* mesh{};

			int total_num_indices = 0;

			// (1) Loop through all the model's meshes
			// -----------------------------------------------------
			std::cout << "\n   scene->mNumMeshes: " << num_meshes;
			std::cout << "\n   ********************\n";
			// ---------------------------------------------------------
			for (unsigned int i = 0; i < num_meshes; ++i) // In this case... scene->mNumMeshes = node->mChildren[i]->mNumMeshes
			{
				mesh_list[i].mesh_num = i; // Fed in to vertex shader as an input attribute (Used to identify and transform meshes independently of one another)
				mesh = scene->mMeshes[i]; // http://assimp.sourceforge.net/lib_html/structai_mesh.html

				std::cout << "\n\n   mesh->mMaterialIndex: " << mesh->mMaterialIndex;
				std::cout << "\n   ----------------------- ";
				std::cout << "\n   mesh->mName.C_Str(): " << mesh->mName.C_Str();

				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex]; // http://assimp.sourceforge.net/lib_html/structai_material.html

				std::cout << "\n\n   material->GetTexture(aiTextureType_DIFFUSE, tex_count, &string): " << material->GetTextureCount(aiTextureType_DIFFUSE);
				std::cout << "\n   material->GetTexture(aiTextureType_SPECULAR, tex_count, &string): " << material->GetTextureCount(aiTextureType_SPECULAR);
				std::cout << "\n   material->GetTexture(aiTextureType_AMBIENT, tex_count, &string): " << material->GetTextureCount(aiTextureType_AMBIENT) << "\n\n";

				unsigned int tex_count = 0; // This loop will only run once (i.e. there's only 1 texture per mesh)
				for (; tex_count < material->GetTextureCount(aiTextureType_DIFFUSE); ++tex_count) // The above std::cout reveals that only using: aiTextureType_DIFFUSE
				{
					aiString string;
					material->GetTexture(aiTextureType_DIFFUSE, tex_count, &string); // Acquire the name of the image file to be loaded.
					std::cout << "   material->GetTexture(aiTextureType_DIFFUSE, tex_count, &string): " << string.C_Str() << "\n\n";

					// (2) Load mesh [i]'s texture if not already loaded
					// ---------------------------------------------------------------
					int already_loaded = is_image_loaded(string.C_Str()); // Returns -1 if texture Not already loaded, otherwise returns Existing texture handle.
					std::cout << "   Loading Image\n";

					if (already_loaded == -1) // Image not yet loaded.
					{
						bool load_complete = false;
						unsigned int texture_handle = load_texture_image(string.C_Str(), load_complete); // Notice the UPPER case "C_Str()"

						if (load_complete) // Although do nothing if the image fails to load.
						{
							Texture texture;
							texture.image_name = string.C_Str();
							texture.tex_handle = texture_handle;
							texture_list.push_back(texture);

							mesh_list[i].tex_handle = texture_handle;
						}
					}
					else // Assign existing texture handle.
					{
						std::string edited = string.C_Str();
						std::size_t position = edited.find_last_of("\\");

						std::cout << "   Image file: " << edited.substr(position + 1) << " (is already loaded)";
						mesh_list[i].tex_handle = already_loaded;
					}
				}
				if (tex_count == 0)
				{
					mesh_list[i].tex_handle = 0;
					// std::cout << "   material->GetTexture(...) No image has been applied to this mesh\n\n";
				}
				else
					std::cout << "\n\n";

				for (unsigned int slot = 0; slot < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++slot)
					std::cout << "   mesh->HasTextureCoords(" << slot << "): " << mesh->HasTextureCoords(slot) << "\n";

				std::cout << "\n   Mesh index: " << i << " (mesh->mNumVertices: " << mesh->mNumVertices << ")";
				std::cout << "\n   ------------------------------------- ";

				// (3) Loop through all mesh [i]'s vertices
				// ---------------------------------------------------
				for (unsigned int i2 = 0; i2 < mesh->mNumVertices; ++i2)
				{
					glm::vec3 position{};
					position.x = mesh->mVertices[i2].x;
					position.y = mesh->mVertices[i2].y;
					position.z = mesh->mVertices[i2].z;
					mesh_list[i].vert_positions.push_back(position);

					std::cout << "\n   Count: " << i2;
					std::cout << "\n   mesh->mVertices[" << i2 << "].x: " << position.x;
					std::cout << "\n   mesh->mVertices[" << i2 << "].y: " << position.y;
					std::cout << "\n   mesh->mVertices[" << i2 << "].z: " << position.z;

					if (mesh->HasNormals())
					{
						glm::vec3 normal{};
						normal.x = mesh->mNormals[i2].x;
						normal.y = mesh->mNormals[i2].y;
						normal.z = mesh->mNormals[i2].z;
						mesh_list[i].vert_normals.push_back(normal);
						std::cout << "\n   mesh->mNormals[" << i2 << "] X: " << normal.x << " Y: " << normal.y << " Z: " << normal.z;
					}
					else
						mesh_list[i].vert_normals.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

					if (mesh->HasTextureCoords(0)) // Above for loop: AI_MAX_NUMBER_OF_TEXTURECOORDS reveals that only slot [0] is in question.
					{
						glm::vec2 tex_coords{};
						tex_coords.x = mesh->mTextureCoords[0][i2].x;
						tex_coords.y = mesh->mTextureCoords[0][i2].y;
						mesh_list[i].tex_coords.push_back(tex_coords);
						std::cout << "\n   mesh->mTextureCoords[0][" << i2 << "] X: " << tex_coords.x << " Y: " << tex_coords.y;
					}
					else
						mesh_list[i].tex_coords.push_back(glm::vec2(0.0f, 0.0f));
				}
				std::cout << "\n\n   mesh->mNumFaces: " << mesh->mNumFaces << "\n";
				std::cout << "   ------------------ ";

				// (4) Loop through all mesh [i]'s Indices
				// --------------------------------------------------
				for (unsigned int i3 = 0; i3 < mesh->mNumFaces; ++i3)
				{
					std::cout << "\n";
					for (unsigned int i4 = 0; i4 < mesh->mFaces[i3].mNumIndices; ++i4)
					{
						std::cout << "   mesh->mFaces[" << i3 << "].mIndices[" << i4 << "]: " << mesh->mFaces[i3].mIndices[i4] << "\n";
						mesh_list[i].vert_indices.push_back(mesh->mFaces[i3].mIndices[i4]);
						++total_num_indices;
					}
				}
				std::cout << "\n   Total number of indices: " << total_num_indices;
				std::cout << "\n   **************************\n";
				total_num_indices = 0;

				set_buffer_data(i); // Set up: VAO, VBO and EBO.
			}
			// Look to see if each mesh's texture handle corresponds correctly to the loaded image
			// ----------------------------------------------------------------------------------------------------------------
			std::cout << "\n   Look to see if each mesh's texture handle corresponds correctly to the loaded image";
			std::cout << "\n   -----------------------------------------------------------------------------------\n";
			if (texture_list.size() > 0)
				for (unsigned int i = 0; i < texture_list.size(); ++i)
				{
					std::cout << "   image_list[" << i << "].imageID: " << texture_list[i].tex_handle << "... image_list[" << i << "].image_name: " << texture_list[i].image_name << "\n";

					for (unsigned int i2 = 0; i2 < num_meshes; ++i2)
						if (texture_list[i].tex_handle == mesh_list[i2].tex_handle)
							std::cout << "   mesh_list[" << i2 << "].tex_handle: " << mesh_list[i2].tex_handle << "\n";
					std::cout << "\n";
				}
			else
				std::cout << "   ***** No images have been loaded\n\n";
		}
	}

	void load_file_name_images(std::vector<Texture>& texture_list)
	{
		if (texture_list.size() > 0)
		{
			for (unsigned int i = 0; i < texture_list.size(); ++i)
			{
				bool load_success = false;
				unsigned int texture_handle = load_texture_image(texture_list[i].image_name, load_success);

				if (load_success)
				{
					texture_list[i].tex_handle = texture_handle; // Parameter: "& texture_list" received by reference and changed here.
					std::cout << " --- load_file_name_images() Successful: " << texture_list[i].image_name << "\n";
				}
				else
					std::cout << " --- load_file_name_images() Failed: " << texture_list[i].image_name << "\n";
			}
		}
		else
			std::cout << "   ***** No images have been loaded\n\n";
	}

	void combine_meshes_5VBO()
	{
		std::cout << "   Analysing combined mesh data\n";
		std::cout << "   ----------------------------";

		unsigned int offset = 0;
		for (unsigned int i = 0; i < num_meshes; ++i) // Combine multiple mesh data into 1 set of vectors.
		{
			// A straightforward copy of the positions, normals and texture coordinates
			// ------------------------------------------------------------------------------------------------
			meshes_5VBO_combined.vert_positions.insert(meshes_5VBO_combined.vert_positions.end(), mesh_list[i].vert_positions.begin(), mesh_list[i].vert_positions.end());
			meshes_5VBO_combined.vert_normals.insert(meshes_5VBO_combined.vert_normals.end(), mesh_list[i].vert_normals.begin(), mesh_list[i].vert_normals.end());
			meshes_5VBO_combined.tex_coords.insert(meshes_5VBO_combined.tex_coords.end(), mesh_list[i].tex_coords.begin(), mesh_list[i].tex_coords.end());

			for (unsigned int i2 = 0; i2 < mesh_list[i].vert_positions.size(); ++i2) // 1 mesh number & 1 sampler position per vertex.
			{
				meshes_5VBO_combined.mesh_num.push_back(mesh_list[i].mesh_num); // Use for identifying and transforming meshes independently.

				bool match_found = false;
				for (unsigned int i3 = 0; i3 < texture_list.size(); ++i3)
					if (texture_list[i3].tex_handle == mesh_list[i].tex_handle) // Compare mesh handle to texture list handle.
					{
						match_found = true;
						meshes_5VBO_combined.sampler_array_pos.push_back(i3);
					}
				if (!match_found) // Note: without assigning some/any pos here in case any of the model meshes don't have a texture applied, then the size of this "sampler_array_pos" results in...
					meshes_5VBO_combined.sampler_array_pos.push_back(0); // ...being too small, and from testing, not surprisingly, the sampler array pos then no longer corresponds correctly.
			}
			for (unsigned int i2 = 0; i2 < mesh_list[i].vert_indices.size(); ++i2) // Offset by the total number of vertices in previous meshes.
				meshes_5VBO_combined.vert_indices.push_back(mesh_list[i].vert_indices[i2] + offset);

			offset += (unsigned int)mesh_list[i].vert_positions.size(); // Unsigned 32 bit is still over 2 billion.
			// std::cout << "\n   Indices offset: " << offset;
		}
		std::cout << "\n\n   Meshes combined into vector lists\n";
		std::cout << "   ---------------------------------\n";
		std::cout << "   meshes_5VBO_combined.vert_positions.size(): " << meshes_5VBO_combined.vert_positions.size() << "\n";
		std::cout << "   meshes_5VBO_combined.vert_normals.size(): " << meshes_5VBO_combined.vert_normals.size() << "\n";
		std::cout << "   meshes_5VBO_combined.tex_coords.size(): " << meshes_5VBO_combined.tex_coords.size() << "\n";
		std::cout << "   meshes_5VBO_combined.mesh_num.size(): " << meshes_5VBO_combined.mesh_num.size() << "\n";
		std::cout << "   meshes_5VBO_combined.vert_indices.size(): " << meshes_5VBO_combined.vert_indices.size() << "\n\n";
	}

	void combine_meshes_1VBO()
	{
		std::cout << "   Analysing combined mesh data\n";
		std::cout << "   ----------------------------";

		Vertex data{};
		data.sampler_array_pos = 0; // Set pos to 0 for when mesh has no texture (See note further down: "data.sampler_array_pos = i3; // Unlike for...")
		unsigned int offset = 0;

		for (unsigned int i = 0; i < num_meshes; ++i)
		{
			for (unsigned int i2 = 0; i2 < mesh_list[i].vert_positions.size(); ++i2) // Add attribute values for every vertex entry.
			{
				data.mesh_num = mesh_list[i].mesh_num; // Use for identifying and transforming meshes independently.

				data.vert_position = mesh_list[i].vert_positions[i2];
				data.vert_normal = mesh_list[i].vert_normals[i2];
				data.tex_coords = mesh_list[i].tex_coords[i2];

				for (unsigned int i3 = 0; i3 < texture_list.size(); ++i3)
					if (texture_list[i3].tex_handle == mesh_list[i].tex_handle) // Compare mesh handle to texture list handle.
						data.sampler_array_pos = i3; // Unlike for "meshes_5VBO_combined.sampler_array_pos.push_back(0)", set pos to 0 further up, for if mesh has no texture.	

				meshes_1VBO_combined.vertex_data.push_back(data); // Add vertex to meshes combined. 	
			}
			for (unsigned int i2 = 0; i2 < mesh_list[i].vert_indices.size(); ++i2) // Offset by the total number of vertices in previous meshes.			
				meshes_1VBO_combined.vert_indices.push_back(mesh_list[i].vert_indices[i2] + offset);

			offset += (unsigned int)mesh_list[i].vert_positions.size(); // Unsigned 32 bit is still over 2 billion.
			// std::cout << "\n   Indices offset: " << offset;
		}
		std::cout << "\n\n   Meshes combined into struct lists\n";
		std::cout << "   ---------------------------------\n";
		std::cout << "   meshes_1VBO_combined.vertex_data.size(): " << meshes_1VBO_combined.vertex_data.size() << "\n\n";
	}

	void set_buffer_data(unsigned index)
	{
		glGenVertexArrays(1, &mesh_list[index].VAO);
		glGenBuffers(1, &mesh_list[index].VBO1);
		glGenBuffers(1, &mesh_list[index].VBO2);
		glGenBuffers(1, &mesh_list[index].VBO3);
		glGenBuffers(1, &mesh_list[index].EBO);

		glBindVertexArray(mesh_list[index].VAO);

		// Vertex Positions
		// ---------------------
		glBindBuffer(GL_ARRAY_BUFFER, mesh_list[index].VBO1);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh_list[index].vert_positions.size(), &mesh_list[index].vert_positions[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // Void pointer below is for legacy reasons. Two possible meanings: "offset for buffer objects" & "address for client state arrays"
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		// Vertex Normals
		// --------------------
		glBindBuffer(GL_ARRAY_BUFFER, mesh_list[index].VBO2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh_list[index].vert_normals.size(), &mesh_list[index].vert_normals[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		// Texture Coordinates
		// ---------------------------
		glBindBuffer(GL_ARRAY_BUFFER, mesh_list[index].VBO3);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mesh_list[index].tex_coords.size(), &mesh_list[index].tex_coords[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

		// Indices for: glDrawElements()
		// ---------------------------------------
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_list[index].EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh_list[index].vert_indices.size(), &mesh_list[index].vert_indices[0], GL_STATIC_DRAW);

		glBindVertexArray(0); // Unbind VAO
	}

	void set_buffer_data_combined_5VBO()
	{
		glGenVertexArrays(1, &meshes_5VBO_combined.VAO);
		glGenBuffers(1, &meshes_5VBO_combined.VBO1); // Alternative to using 5 separate VBOs... see function: set_buffer_data_combined_1VBO()
		glGenBuffers(1, &meshes_5VBO_combined.VBO2);
		glGenBuffers(1, &meshes_5VBO_combined.VBO3);
		glGenBuffers(1, &meshes_5VBO_combined.VBO4);
		glGenBuffers(1, &meshes_5VBO_combined.VBO5);
		glGenBuffers(1, &meshes_5VBO_combined.EBO);

		glBindVertexArray(meshes_5VBO_combined.VAO);

		// Vertex Positions
		// ---------------------
		glBindBuffer(GL_ARRAY_BUFFER, meshes_5VBO_combined.VBO1);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * meshes_5VBO_combined.vert_positions.size(), &meshes_5VBO_combined.vert_positions[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // Void pointer below is for legacy reasons. Two possible meanings: "offset for buffer objects" & "address for client state arrays"
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		// Vertex Normals
		// --------------------
		glBindBuffer(GL_ARRAY_BUFFER, meshes_5VBO_combined.VBO2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * meshes_5VBO_combined.vert_normals.size(), &meshes_5VBO_combined.vert_normals[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		// Texture Coordinates
		// ---------------------------
		glBindBuffer(GL_ARRAY_BUFFER, meshes_5VBO_combined.VBO3);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * meshes_5VBO_combined.tex_coords.size(), &meshes_5VBO_combined.tex_coords[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

		// Mesh Number
		// -------------------
		glBindBuffer(GL_ARRAY_BUFFER, meshes_5VBO_combined.VBO4);
		glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * meshes_5VBO_combined.mesh_num.size(), &meshes_5VBO_combined.mesh_num[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(3);
		glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, 0, (void*)0); // (Notice the "I")

		// Sampler Array Position
		// ------------------------------
		glBindBuffer(GL_ARRAY_BUFFER, meshes_5VBO_combined.VBO5);
		glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * meshes_5VBO_combined.sampler_array_pos.size(), &meshes_5VBO_combined.sampler_array_pos[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(4);
		glVertexAttribIPointer(4, 1, GL_UNSIGNED_INT, 0, (void*)0); // (Notice the "I")

		// Indices for: glDrawElements()
		// ---------------------------------------
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshes_5VBO_combined.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * meshes_5VBO_combined.vert_indices.size(), &meshes_5VBO_combined.vert_indices[0], GL_STATIC_DRAW);

		glBindVertexArray(0); // Unbind VAO
	}

	void set_buffer_data_combined_1VBO()
	{
		glGenVertexArrays(1, &meshes_1VBO_combined.VAO);
		glGenBuffers(1, &meshes_1VBO_combined.VBO);
		glGenBuffers(1, &meshes_1VBO_combined.EBO);

		glBindVertexArray(meshes_1VBO_combined.VAO);

		// Vertex Positions
		// ---------------------
		glBindBuffer(GL_ARRAY_BUFFER, meshes_1VBO_combined.VBO);
		glBufferData(GL_ARRAY_BUFFER, meshes_1VBO_combined.vertex_data.size() * sizeof(Vertex), &meshes_1VBO_combined.vertex_data[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // Void pointer below is for legacy reasons. Two possible meanings: "offset for buffer objects" & "address for client state arrays"
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		// Vertex Normals
		// --------------------
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, vert_normal));

		// Texture Coordinates
		// ---------------------------
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));

		// Mesh Number
		// -------------------
		glEnableVertexAttribArray(3);
		glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(Vertex), (void*)offsetof(Vertex, mesh_num)); // Notice the "I"

		// Sampler Array Position
		// ------------------------------	
		glEnableVertexAttribArray(4);
		glVertexAttribIPointer(4, 1, GL_UNSIGNED_INT, sizeof(Vertex), (void*)offsetof(Vertex, sampler_array_pos)); // Notice the "I"

		// Indices for: glDrawElements()
		// ---------------------------------------
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshes_1VBO_combined.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * meshes_1VBO_combined.vert_indices.size(), &meshes_1VBO_combined.vert_indices[0], GL_STATIC_DRAW);

		glBindVertexArray(0); // Unbind VAO		
	}

	int is_image_loaded(std::string image_path)
	{
		for (unsigned int i = 0; i < texture_list.size(); ++i)
			if (image_path.compare(texture_list[i].image_name) == 0)
				return texture_list[i].tex_handle;
		return -1;
	}

	unsigned int load_texture_image(std::string image_path, bool& load_complete)
	{
		// stbi_set_flip_vertically_on_load(1); // Call this function if the image is upside-down.		

		std::size_t position = image_path.find_last_of("\\");
		image_path = "Images\\" + image_path.substr(position + 1);

		int width, height, num_components;
		unsigned char* image_data = stbi_load(image_path.c_str(), &width, &height, &num_components, 0);

		glActiveTexture(GL_TEXTURE0); // Reset to OpenGL's default texture unit 0 (Although not actually necessary because models are loaded 1st)

		unsigned int tex_handle;
		glGenTextures(1, &tex_handle);

		if (image_data)
		{
			GLenum format{};

			if (num_components == 1)
				format = GL_RED;
			else if (num_components == 3)
				format = GL_RGB;
			else if (num_components == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, tex_handle);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Recommended by NVIDIA Rep: https://devtalk.nvidia.com/default/topic/875205/opengl/how-does-gl_unpack_alignment-work-/

			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image_data);
			glGenerateMipmap(GL_TEXTURE_2D);

			// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexParameter.xhtml
			// ----------------------------------------------------------------------------------------------------------------
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // GL_REPEAT... GL_MIRRORED_REPEAT... GL_CLAMP_TO_EDGE... GL_CLAMP_TO_BORDER.
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			// float border_colour[] = {0.45, 0.55, 0.95};
			// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_colour); // For above when using: GL_CLAMP_TO_BORDER		

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_NEAREST... GL_LINEAR... GL_NEAREST_MIPMAP_NEAREST (See above link for full list)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // GL_NEAREST or GL_LINEAR.

			load_complete = true;
			stbi_image_free(image_data);
			std::cout << "   Image loaded OK: " << image_path;
		}
		else
		{
			load_complete = false;
			stbi_image_free(image_data);
			std::cout << "   Image failed to load: " << image_path;
		}
		return tex_handle;
	}

	void draw_multiple_meshes(unsigned buffer_width, unsigned buffer_height)
	{
		glUniform1i(meshes_combined_loc, 0);
		glUniform1i(draw_multiple_meshes_sampler_pos0_val, 0); // Make sure sampler array is at position 0: image[0] used in fragment shader, is set to 0.

		glActiveTexture(GL_TEXTURE0);
		glViewport(0, 0, buffer_width, buffer_height);

		for (unsigned int i = 0; i < num_meshes; ++i)
		{
			glUniform1i(rendering_multiple_meshes_loc, i);

			glBindTexture(GL_TEXTURE_2D, mesh_list[i].tex_handle); // Bind texture for the current mesh.

			glBindVertexArray(mesh_list[i].VAO);
			glDrawElements(GL_TRIANGLES, (GLsizei)mesh_list[i].vert_indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}

	void draw_meshes_combined_5VBO(unsigned buffer_width, unsigned buffer_height)
	{
		glUniform1i(meshes_combined_loc, 1); // If-statement in fragment shader.
		glUniform1i(rendering_multiple_meshes_loc, -1);

		if (binary_version_found)
			populate_sampler_array(meshes_5VBO_combined.texture_list);
		else
			populate_sampler_array(texture_list);

		glViewport(0, 0, buffer_width, buffer_height);

		glBindVertexArray(meshes_5VBO_combined.VAO);
		glDrawElements(GL_TRIANGLES, (GLsizei)meshes_5VBO_combined.vert_indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void draw_meshes_combined_1VBO(unsigned buffer_width, unsigned buffer_height)
	{
		glUniform1i(meshes_combined_loc, 1); // If-statement in fragment shader.
		glUniform1i(rendering_multiple_meshes_loc, -1);

		if (binary_version_found)
			populate_sampler_array(meshes_1VBO_combined.texture_list);
		else
			populate_sampler_array(texture_list);

		glViewport(0, 0, buffer_width, buffer_height);

		glBindVertexArray(meshes_1VBO_combined.VAO);
		glDrawElements(GL_TRIANGLES, (GLsizei)meshes_1VBO_combined.vert_indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void populate_sampler_array(std::vector<Texture> texture_list)
	{
		for (unsigned int i = 0; i < texture_list.size(); ++i)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, texture_list[i].tex_handle);

			glUniform1i(texture_list[i].sampler_location, i);
		}
	}

	bool look_for_model_5VBO(std::string read_path)
	{
		bool file_not_found = false;

		std::ifstream vert_data; // Read data.

		std::size_t position = read_path.find_last_of("\\");
		std::string model_name = read_path.substr(position + 1);

		vert_data.open("Model Data 5VBO/" + model_name + "_vert_positions.bin", std::ios::in);
		std::cout << "\n   Detecting: " << model_name + "_vert_positions.bin: " << vert_data.is_open() << "\n";
		if (!vert_data.is_open()) { file_not_found = true; }
		vert_data.close();

		vert_data.open("Model Data 5VBO/" + model_name + "_vert_normals.bin", std::ios::in);
		std::cout << "   Detecting: " << model_name + "_vert_normals.bin: " << vert_data.is_open() << "\n";
		if (!vert_data.is_open()) { file_not_found = true; }
		vert_data.close();

		vert_data.open("Model Data 5VBO/" + model_name + "_tex_coords.bin", std::ios::in);
		std::cout << "   Detecting: " << model_name + "_tex_coords.bin: " << vert_data.is_open() << "\n";
		if (!vert_data.is_open()) { file_not_found = true; }
		vert_data.close();

		vert_data.open("Model Data 5VBO/" + model_name + "_vert_indices.bin", std::ios::in);
		std::cout << "   Detecting: " << model_name + "_vert_indices.bin: " << vert_data.is_open() << "\n";
		if (!vert_data.is_open()) { file_not_found = true; }
		vert_data.close();

		vert_data.open("Model Data 5VBO/" + model_name + "_mesh_num.bin", std::ios::in);
		std::cout << "   Detecting: " << model_name + "_mesh_num.bin: " << vert_data.is_open() << "\n";
		if (!vert_data.is_open()) { file_not_found = true; }
		vert_data.close();

		vert_data.open("Model Data 5VBO/" + model_name + "_sampler_array_pos.bin", std::ios::in);
		std::cout << "   Detecting: " << model_name + "_sampler_array_pos.bin: " << vert_data.is_open() << "\n";
		if (!vert_data.is_open()) { file_not_found = true; }
		vert_data.close();

		vert_data.open("Model Data 5VBO/" + model_name + "_image_names.txt", std::ios::in);
		std::cout << "   Detecting: " << model_name + "_image_names.txt: " << vert_data.is_open() << "\n";
		if (!vert_data.is_open()) { file_not_found = true; }
		vert_data.close();

		if (file_not_found)
			return false;

		return true;
	}

	void write_model_files_5VBO(std::string write_path)
	{
		std::string file_name;
		std::ofstream vert_data; // Write data.

		std::size_t position = write_path.find_last_of("\\");
		std::string model_name = write_path.substr(position + 1);

		file_name = "Model Data 5VBO/" + model_name + "_vert_positions.bin";
		vert_data.open(file_name, std::ios::out | std::ios::binary);
		vert_data.write(reinterpret_cast<char*>(&meshes_5VBO_combined.vert_positions[0]), meshes_5VBO_combined.vert_positions.size() * 3 * sizeof(float));
		vert_data.close();

		file_name = "Model Data 5VBO/" + model_name + "_vert_normals.bin";
		vert_data.open(file_name, std::ios::out | std::ios::binary);
		vert_data.write(reinterpret_cast<char*>(&meshes_5VBO_combined.vert_normals[0]), meshes_5VBO_combined.vert_normals.size() * 3 * sizeof(float));
		vert_data.close();

		file_name = "Model Data 5VBO/" + model_name + "_tex_coords.bin";
		vert_data.open(file_name, std::ios::out | std::ios::binary);
		vert_data.write(reinterpret_cast<char*>(&meshes_5VBO_combined.tex_coords[0]), meshes_5VBO_combined.tex_coords.size() * 2 * sizeof(float));
		vert_data.close();

		file_name = "Model Data 5VBO/" + model_name + "_vert_indices.bin";
		vert_data.open(file_name, std::ios::out | std::ios::binary);
		vert_data.write(reinterpret_cast<char*>(&meshes_5VBO_combined.vert_indices[0]), meshes_5VBO_combined.vert_indices.size() * sizeof(unsigned int));
		vert_data.close();

		file_name = "Model Data 5VBO/" + model_name + "_mesh_num.bin";
		vert_data.open(file_name, std::ios::out | std::ios::binary);
		vert_data.write(reinterpret_cast<char*>(&meshes_5VBO_combined.mesh_num[0]), meshes_5VBO_combined.mesh_num.size() * sizeof(unsigned int));
		vert_data.close();

		file_name = "Model Data 5VBO/" + model_name + "_sampler_array_pos.bin";
		vert_data.open(file_name, std::ios::out | std::ios::binary);
		vert_data.write(reinterpret_cast<char*>(&meshes_5VBO_combined.sampler_array_pos[0]), meshes_5VBO_combined.sampler_array_pos.size() * sizeof(unsigned int));
		vert_data.close();

		std::string names_to_file;
		for (unsigned int i = 0; i < texture_list.size(); ++i)
		{
			names_to_file += texture_list[i].image_name;
			names_to_file += "\n";
		}
		// file_name = "Model Data 5VBO/" + model_name + "_image_names.bin";
		// vert_data.open(file_name, std::ios::out | std::ios::binary);
		file_name = "Model Data 5VBO/" + model_name + "_image_names.txt";
		vert_data.open(file_name, std::ios::out);
		vert_data.write(reinterpret_cast<char*>(&names_to_file[0]), names_to_file.size() * sizeof(char));
		vert_data.close();
	}

	void read_model_files_5VBO(std::string read_path)
	{
		std::string file_name;
		std::ifstream vert_data; // Read data.
		size_t vector_bytes_size = 0;
		size_t vector_index_size = 0;

		std::size_t position = read_path.find_last_of("\\");
		std::string model_name = read_path.substr(position + 1);
		file_name = model_name;

		// -------------------

		file_name = "Model Data 5VBO/" + model_name + "_vert_positions.bin";
		vert_data.open(file_name, std::ios::in | std::ios::binary | std::ios::ate);

		vector_bytes_size = vert_data.tellg();
		vector_index_size = vector_bytes_size / (3 * sizeof(float));

		meshes_5VBO_combined.vert_positions.resize(vector_index_size);

		vert_data.seekg(std::ios::beg);
		vert_data.read(reinterpret_cast<char*>(&meshes_5VBO_combined.vert_positions[0]), vector_bytes_size);
		vert_data.close();

		// -------------------

		file_name = "Model Data 5VBO/" + model_name + "_vert_normals.bin";
		vert_data.open(file_name, std::ios::in | std::ios::binary | std::ios::ate);

		vector_bytes_size = vert_data.tellg();
		vector_index_size = vector_bytes_size / (3 * sizeof(float));

		meshes_5VBO_combined.vert_normals.resize(vector_index_size);

		vert_data.seekg(std::ios::beg);
		vert_data.read(reinterpret_cast<char*>(&meshes_5VBO_combined.vert_normals[0]), vector_bytes_size);
		vert_data.close();

		// -------------------

		file_name = "Model Data 5VBO/" + model_name + "_tex_coords.bin";
		vert_data.open(file_name, std::ios::in | std::ios::binary | std::ios::ate);

		vector_bytes_size = vert_data.tellg();
		vector_index_size = vector_bytes_size / (2 * sizeof(float));

		meshes_5VBO_combined.tex_coords.resize(vector_index_size);

		vert_data.seekg(std::ios::beg);
		vert_data.read(reinterpret_cast<char*>(&meshes_5VBO_combined.tex_coords[0]), vector_bytes_size);
		vert_data.close();

		// -------------------

		file_name = "Model Data 5VBO/" + model_name + "_vert_indices.bin";
		vert_data.open(file_name, std::ios::in | std::ios::binary | std::ios::ate);

		vector_bytes_size = vert_data.tellg();
		vector_index_size = vector_bytes_size / sizeof(unsigned int);

		meshes_5VBO_combined.vert_indices.resize(vector_index_size);

		vert_data.seekg(std::ios::beg);
		vert_data.read(reinterpret_cast<char*>(&meshes_5VBO_combined.vert_indices[0]), vector_bytes_size);
		vert_data.close();

		// -------------------

		file_name = "Model Data 5VBO/" + model_name + "_mesh_num.bin";
		vert_data.open(file_name, std::ios::in | std::ios::binary | std::ios::ate);

		vector_bytes_size = vert_data.tellg();
		vector_index_size = vector_bytes_size / sizeof(unsigned int);

		meshes_5VBO_combined.mesh_num.resize(vector_index_size);

		vert_data.seekg(std::ios::beg);
		vert_data.read(reinterpret_cast<char*>(&meshes_5VBO_combined.mesh_num[0]), vector_bytes_size);
		vert_data.close();

		// -------------------

		file_name = "Model Data 5VBO/" + model_name + "_sampler_array_pos.bin";
		vert_data.open(file_name, std::ios::in | std::ios::binary | std::ios::ate);

		vector_bytes_size = vert_data.tellg();
		vector_index_size = vector_bytes_size / sizeof(unsigned int);

		meshes_5VBO_combined.sampler_array_pos.resize(vector_index_size);

		vert_data.seekg(std::ios::beg);
		vert_data.read(reinterpret_cast<char*>(&meshes_5VBO_combined.sampler_array_pos[0]), vector_bytes_size);
		vert_data.close();

		// -------------------		

			// file_name = "Model Data 5VBO/" + model_name + "_image_names.bin";
			// vert_data.open(file_name, std::ios::in | std::ios::binary | std::ios::ate);
		file_name = "Model Data 5VBO/" + model_name + "_image_names.txt";
		vert_data.open(file_name, std::ios::in | std::ios::ate);

		vector_bytes_size = vert_data.tellg();
		vector_index_size = vector_bytes_size / sizeof(char);

		std::string names_from_file;
		names_from_file.resize(vector_index_size);

		vert_data.seekg(std::ios::beg);
		vert_data.read(reinterpret_cast<char*>(&names_from_file[0]), vector_bytes_size);
		vert_data.close();

		const char* all_names = names_from_file.c_str();

		std::string single_name;
		for (unsigned int i = 0; i < names_from_file.size(); ++i)
		{
			if (*all_names != '\n')
				single_name += *all_names;
			else
			{
				Texture texture;
				texture.image_name = single_name;
				texture.tex_handle = 0; // Temporary value. Gets overridden in: load_file_name_images()

				meshes_5VBO_combined.texture_list.push_back(texture);
				single_name.clear();
			}
			++all_names;
		}
		/*for (unsigned int i = 0; i < meshes_5VBO_combined.texture_list.size(); ++i)
			std::cout << "\n Retrieved names: " << meshes_5VBO_combined.texture_list[i].image_name << "\n";*/
	}

	bool look_for_model_1VBO(std::string read_path)
	{
		bool file_not_found = false;

		std::ifstream vert_data; // Read data.

		std::size_t position = read_path.find_last_of("\\");
		std::string model_name = read_path.substr(position + 1);

		vert_data.open("Model Data 1VBO/" + model_name + "_vertex_data.bin", std::ios::in);
		std::cout << "\n   Detecting: " << model_name + "_vertex_data.bin: " << vert_data.is_open() << "\n";
		if (!vert_data.is_open()) { file_not_found = true; }
		vert_data.close();

		vert_data.open("Model Data 1VBO/" + model_name + "_vert_indices.bin", std::ios::in);
		std::cout << "   Detecting: " << model_name + "_vert_indices.bin: " << vert_data.is_open() << "\n";
		if (!vert_data.is_open()) { file_not_found = true; }
		vert_data.close();

		vert_data.open("Model Data 1VBO/" + model_name + "_image_names.txt", std::ios::in);
		std::cout << "   Detecting: " << model_name + "_image_names.txt: " << vert_data.is_open() << "\n";
		if (!vert_data.is_open()) { file_not_found = true; }
		vert_data.close();

		if (file_not_found)
			return false;

		return true;
	}

	void write_model_files_1VBO(std::string write_path)
	{
		std::string file_name;
		std::ofstream vert_data; // Write data.

		std::size_t position = write_path.find_last_of("\\");
		std::string model_name = write_path.substr(position + 1);

		file_name = "Model Data 1VBO/" + model_name + "_vertex_data.bin";
		vert_data.open(file_name, std::ios::out | std::ios::binary);
		vert_data.write(reinterpret_cast<char*>(&meshes_1VBO_combined.vertex_data[0]), meshes_1VBO_combined.vertex_data.size() * sizeof(Vertex));
		vert_data.close();

		file_name = "Model Data 1VBO/" + model_name + "_vert_indices.bin";
		vert_data.open(file_name, std::ios::out | std::ios::binary);
		vert_data.write(reinterpret_cast<char*>(&meshes_1VBO_combined.vert_indices[0]), meshes_1VBO_combined.vert_indices.size() * sizeof(unsigned int));
		vert_data.close();

		std::string names_to_file;
		for (unsigned int i = 0; i < texture_list.size(); ++i)
		{
			names_to_file += texture_list[i].image_name;
			names_to_file += "\n";
		}
		// file_name = "Model Data 1VBO/" + model_name + "_image_names.bin";
		// vert_data.open(file_name, std::ios::out | std::ios::binary);
		file_name = "Model Data 1VBO/" + model_name + "_image_names.txt";
		vert_data.open(file_name, std::ios::out);
		vert_data.write(reinterpret_cast<char*>(&names_to_file[0]), names_to_file.size() * sizeof(char));
		vert_data.close();
	}

	void read_model_files_1VBO(std::string read_path)
	{
		std::string file_name;
		std::ifstream vert_data; // Read data.
		size_t vector_bytes_size = 0;
		size_t vector_index_size = 0;

		std::size_t position = read_path.find_last_of("\\");
		std::string model_name = read_path.substr(position + 1);
		file_name = model_name;

		// -------------------

		file_name = "Model Data 1VBO/" + model_name + "_vertex_data.bin";
		vert_data.open(file_name, std::ios::in | std::ios::binary | std::ios::ate);

		vector_bytes_size = vert_data.tellg();
		vector_index_size = vector_bytes_size / (3 * sizeof(float));

		meshes_1VBO_combined.vertex_data.resize(vector_index_size);

		vert_data.seekg(std::ios::beg);
		vert_data.read(reinterpret_cast<char*>(&meshes_1VBO_combined.vertex_data[0]), vector_bytes_size);
		vert_data.close();

		// -------------------

		file_name = "Model Data 1VBO/" + model_name + "_vert_indices.bin";
		vert_data.open(file_name, std::ios::in | std::ios::binary | std::ios::ate);

		vector_bytes_size = vert_data.tellg();
		vector_index_size = vector_bytes_size / sizeof(unsigned int);

		meshes_1VBO_combined.vert_indices.resize(vector_index_size);

		vert_data.seekg(std::ios::beg);
		vert_data.read(reinterpret_cast<char*>(&meshes_1VBO_combined.vert_indices[0]), vector_bytes_size);
		vert_data.close();

		// -------------------

			// file_name = "Model Data 1VBO/" + model_name + "_image_names.bin";
			// vert_data.open(file_name, std::ios::in | std::ios::binary | std::ios::ate);
		file_name = "Model Data 1VBO/" + model_name + "_image_names.txt";
		vert_data.open(file_name, std::ios::in | std::ios::ate);

		vector_bytes_size = vert_data.tellg();
		vector_index_size = vector_bytes_size / sizeof(char);

		std::string names_from_file;
		names_from_file.resize(vector_index_size);

		vert_data.seekg(std::ios::beg);
		vert_data.read(reinterpret_cast<char*>(&names_from_file[0]), vector_bytes_size);
		vert_data.close();

		const char* all_names = names_from_file.c_str();

		std::string single_name;
		for (unsigned int i = 0; i < names_from_file.size(); ++i)
		{
			if (*all_names != '\n')
				single_name += *all_names;
			else
			{
				Texture texture;
				texture.image_name = single_name;
				texture.tex_handle = 0; // Temporary value. Gets overridden in: load_file_name_images()

				meshes_1VBO_combined.texture_list.push_back(texture);
				single_name.clear();
			}
			++all_names;
		}
	}
};
Source code : C++ from... shadow_maps.h
#pragma once

class Shadow
{
private:
	int texture_unit;
	// ------------------
	unsigned frame_buffer;
	unsigned depth_values;
	unsigned map_width;
	unsigned map_height;
	// --------------------------	
	unsigned light_count_loc;
	unsigned light_position_loc;
	unsigned light_view_matrix_loc;
	unsigned transform_light_pos_mat_loc;
	unsigned cam_far_plane_loc;
	unsigned cam_near_plane_loc;
	// --------------------------------------	

public:
	const int ID; // Used in: initialise_light_view_parameters()
	bool clear_depth_buffer_flag;

	Shadow(Shader& main_shader, glm::vec3 light_position, glm::vec3 light_target, float FOV) : ID(counter()++) // Perspective.
	{
		create_shadow_map();
		initialise_light_view_parameters(main_shader);
		update_light_view_perspective(light_position, light_target, FOV);
	}

	Shadow(Shader& main_shader, glm::vec3 light_position, glm::vec3 light_target, float left, float right, float bottom, float top) : ID(counter()++) // Orthographic.
	{
		create_shadow_map();
		initialise_light_view_parameters(main_shader);
		update_light_view_orthographic(light_position, light_target, left, right, bottom, top);
	}

	void update_light_view_perspective(glm::vec3 position, glm::vec3 target, float FOV)
	{
		calculate_new_light_position(position, target); // Note: All lights drawn in Blender are at: (0, 0, 0)

		// https://developer.nvidia.com/content/depth-precision-visualized (Includes graphs to help explain the nature of the... 1/z depth buffer)
		// Make sure the "near plane" is as close as possible to the objects casting shadows, or else the high-resolution region of the depth buffer will be wasted!
		// Particularly the near-plane being as close as possible is important, but it also helps if the far-plane is calculated as tightly as possible too.		
		// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		float far_plane = 60.0f;
		float near_plane = 7.0f; // If near-plane is much below 3.0, then shadow depth testing becomes virtually uncontrollable.
		float plane_bias = 0.0005f;

		glm::vec3 up_vector(0.0f, 1.0f, 0.0f);

		glm::mat4 view = glm::lookAt(position, target, up_vector);
		glm::mat4 perspective_projection = glm::perspective(glm::radians(FOV), 1.0f, near_plane, far_plane); // Use a square aspect ratio (1.0f) for shadow's light source.		

		glm::mat4 light_view_matrix = perspective_projection * view;
		glUniformMatrix4fv(light_view_matrix_loc, 1, GL_FALSE, glm::value_ptr(light_view_matrix));

		// Note: the plane width and height (unlike their positions which are transformed as below) are not being set dynamically, and are rendered simply as drawn in Blender
		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		glm::mat4 cam_far_plane(1.0f);
		cam_far_plane = glm::translate(cam_far_plane, glm::vec3(0.0f, -far_plane + plane_bias, 0.0f));
		glUniformMatrix4fv(cam_far_plane_loc, 1, GL_FALSE, glm::value_ptr(cam_far_plane));

		glm::mat4 cam_near_plane(1.0f);
		cam_near_plane = glm::translate(cam_near_plane, glm::vec3(0.0f, -near_plane + plane_bias, 0.0f));
		glUniformMatrix4fv(cam_near_plane_loc, 1, GL_FALSE, glm::value_ptr(cam_near_plane));
	}

	void update_light_view_orthographic(glm::vec3 position, glm::vec3 target, float left, float right, float bottom, float top)
	{
		calculate_new_light_position(position, target); // Note: All lights drawn in Blender are at: (0, 0, 0)

		float far_plane = 50.0f;
		float near_plane = 7.0f; // If near-plane is much below 3.0, then shadow depth testing becomes virtually uncontrollable.
		float plane_bias = 0.0005f;

		glm::vec3 up_vector(0.0f, 1.0f, 0.0f);

		glm::mat4 view = glm::lookAt(position, target, up_vector);
		glm::mat4 orthographic_projection = glm::ortho(left, right, bottom, top, near_plane, far_plane);

		glm::mat4 light_view_matrix = orthographic_projection * view;
		glUniformMatrix4fv(light_view_matrix_loc, 1, GL_FALSE, glm::value_ptr(light_view_matrix));

		// Note: the plane width and height (unlike their positions which are transformed as below) are not being set dynamically, and are rendered simply as drawn in Blender
		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		glm::mat4 cam_far_plane(1.0f);
		cam_far_plane = glm::translate(cam_far_plane, glm::vec3(0.0f, -far_plane + plane_bias, 0.0f));
		glUniformMatrix4fv(cam_far_plane_loc, 1, GL_FALSE, glm::value_ptr(cam_far_plane));

		glm::mat4 cam_near_plane(1.0f);
		cam_near_plane = glm::translate(cam_near_plane, glm::vec3(0.0f, -near_plane + plane_bias, 0.0f));
		glUniformMatrix4fv(cam_near_plane_loc, 1, GL_FALSE, glm::value_ptr(cam_near_plane));
	}

	void render_model_shadow(Model& model, bool clear_depth_bit)
	{
		glViewport(0, 0, map_width, map_height);
		glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

		if (clear_depth_bit)
			glClear(GL_DEPTH_BUFFER_BIT);

		model.process_draw_calls(map_width, map_height); // Calling model class public function.

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

private:
	static int& counter()
	{
		static int counter = 0;
		return counter;
	}

	void create_shadow_map()
	{
		// https://stackoverflow.com/questions/22277063/glsl-texture-mapping-max-number-of-textures (See 2nd post)
		// "GL_MAX_TEXTURE_IMAGE_UNITS returns the count for fragment shaders only" (https://www.khronos.org/opengl/wiki/Common_Mistakes Under: Texture Unit)			
		// Check maximum texture units allowed: https://www.khronos.org/opengl/wiki/Shader#Resource_limitations		
		// ---------------------------------------------------
		GLint max_combined_texture_units;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_combined_texture_units);
		std::cout << "   The total number of texture image units that can be used (All active programs stages) = " << max_combined_texture_units << "\n";

		GLint max_fragment_shader_texture_units;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_fragment_shader_texture_units);
		std::cout << "   The maximum number of texture image units that the sampler can use (Fragment shader) = " << max_fragment_shader_texture_units << "\n\n";

		// Counting down from the maximum is a simple way for the shadow texture units to avoid the model texture units (Therefore... for 2 lights, no single model should exceed 30 texture images)
		texture_unit = max_fragment_shader_texture_units - 1 - ID; // Counting down from a maximum of 32 on my PC (GL_TEXTURE0 to GL_TEXTURE31)

		glActiveTexture(GL_TEXTURE0 + texture_unit);

		map_width = 1500; // Shadow map render width & height.
		map_height = 1500;

		// Create texture for storing depth values
		// ---------------------------------------------------
		glGenTextures(1, &depth_values);
		glBindTexture(GL_TEXTURE_2D, depth_values);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, map_width, map_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); // Stops the shadowing being repeated in a grid-like manner.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		// ------------------------------------------------------------------------------------------------------------
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		float border_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

		// Attach depth values texture to frame-buffer
		// ---------------------------------------------------------
		glGenFramebuffers(1, &frame_buffer); // Generate frame-buffer instead of using OpenGL's default frame-buffer.

		glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_values, 0);

		glDrawBuffer(GL_NONE); // Disabled because only the depth value is required via the fragment shader processing stage...
		glReadBuffer(GL_NONE); // which happens automatically within the graphics pipeline.

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void initialise_light_view_parameters(Shader& shader)
	{
		std::string index_num = std::to_string(ID); // ID starts at 0 within counter()

		std::string sampler_name = "shadow_map[" + index_num + "]";
		glUniform1i(glGetUniformLocation(shader.ID, sampler_name.c_str()), texture_unit); // Used in the fragment shader.		

		std::string camera_far_plane = "camera_far_plane[" + index_num + "]";
		cam_far_plane_loc = glGetUniformLocation(shader.ID, camera_far_plane.c_str()); // Used in the vertex shader.

		std::string camera_near_plane = "camera_near_plane[" + index_num + "]";
		cam_near_plane_loc = glGetUniformLocation(shader.ID, camera_near_plane.c_str()); // Used in the vertex shader.

		std::string light_position = "light_position[" + index_num + "]";
		light_position_loc = glGetUniformLocation(shader.ID, light_position.c_str()); // Used in the fragment shader.

		std::string light_view_matrix = "light_view_matrix[" + index_num + "]";
		light_view_matrix_loc = glGetUniformLocation(shader.ID, light_view_matrix.c_str()); // Used in the vertex shader.

		std::string transform_light_pos = "transform_light_pos_mat[" + index_num + "]";
		transform_light_pos_mat_loc = glGetUniformLocation(shader.ID, transform_light_pos.c_str()); // Used in the vertex shader.
	}

	void calculate_new_light_position(glm::vec3 position, glm::vec3 target)
	{
		// Note: All lights drawn in Blender are at: (0, 0, 0)		

		glm::vec3 view_direction = glm::normalize(position - target);

		if (view_direction.x == 0 && view_direction.z == 0) // Avoids glm::lookAt() crashing via cross-product when view direction is parallel to up-vector.
		{
			view_direction.z += 0.00001f; // Slightly change either X or Z here for either position or target.
			view_direction = glm::normalize(view_direction);
		}
		glm::vec3 up_vector(0.0f, 1.0f, 0.0f);

		glm::mat4 shadow_cam_trans(1.0f);
		shadow_cam_trans = glm::translate(shadow_cam_trans, position);

		glm::vec3 cam_rot_axis = glm::normalize(glm::cross(view_direction, up_vector));
		float cam_rot_angle_deg = -glm::acos(glm::dot(view_direction, up_vector)) * 57.2958f;

		glm::mat4 shadow_cam_rot(1.0f); // Convert to degrees below simply in case std::cout to console window.
		shadow_cam_rot = glm::rotate(shadow_cam_rot, glm::radians(cam_rot_angle_deg), cam_rot_axis);

		glm::vec3 view_dir_proj_XZ = glm::normalize(glm::vec3(view_direction.x, 0.0f, view_direction.z));
		float view_dir_spin_angle_deg = glm::acos(glm::dot(view_dir_proj_XZ, glm::vec3(0.0f, 0.0f, -1.0f))) * 57.2958f;

		if (view_direction.x > 0)
			view_dir_spin_angle_deg = -view_dir_spin_angle_deg;

		// std::cout << "   view_direction.x: " << view_direction.x << " --- view_dir_spin_angle_deg: " << view_dir_spin_angle_deg << "\n";
		shadow_cam_rot = glm::rotate(shadow_cam_rot, glm::radians(view_dir_spin_angle_deg), glm::vec3(0, 1, 0));

		glm::mat4 shadow_cam_pos = shadow_cam_trans * shadow_cam_rot;
		glUniformMatrix4fv(transform_light_pos_mat_loc, 1, GL_FALSE, glm::value_ptr(shadow_cam_pos));

		glUniform3f(light_position_loc, position.x, position.y, position.z); // Used in fragment shader.
	}
};
Source code : C++ from... window_params.h
#pragma once

class Window_Params
{
public:
	// Accessed in: main.cpp
	// -----------------------------
	int init_window_width;
	int init_window_height;

	int frame_buffer_width;
	int frame_buffer_height;

	// Accessed in: callback_functions.h
	// --------------------------------------------
	GLFWwindow* window = nullptr;
	const GLFWvidmode* mode = nullptr;

	int curr_window_width;
	int curr_window_height;

	int monitor_width;
	int monitor_height;

	int width_offset;
	int height_offset;

	bool in_fullscreen_mode = false;

	Window_Params()
	{
		// GLFW: Initialise & Configure
		// -------------------------------------
		if (!glfwInit())
			exit(EXIT_FAILURE);

		glfwWindowHint(GLFW_SAMPLES, 4); // Anti-aliasing
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		monitor_width = mode->width; // Monitor's width.
		monitor_height = mode->height;

		init_window_width = (int)(monitor_width * 0.65f); // Window size will be 85% the monitor's size.
		init_window_height = (int)(monitor_height * 0.65f); // Cast is simply to silence the compiler warning.

		curr_window_width = init_window_width;
		curr_window_height = init_window_height;

		window = glfwCreateWindow(init_window_width, init_window_height, "GLFW Keys Mouse Joystick & Window Controls", NULL, NULL);

		if (!window)
		{
			glfwTerminate();
			exit(EXIT_FAILURE);
		}
		width_offset = (monitor_width - init_window_width) / 2;
		height_offset = (monitor_height - init_window_height) / 2;

		glfwMakeContextCurrent(window);
		glfwSetWindowPos(window, width_offset, height_offset); // Set the window to be used and then centre that window on the monitor.		

		glfwSwapInterval(1); // Set VSync rate 1:1 with monitor's refresh rate.

		glfwGetFramebufferSize(window, &frame_buffer_width, &frame_buffer_height);

		// GLAD: Load OpenGL Function Pointers
		// ---------------------------------------------------
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // For GLAD 2 use the following instead: gladLoadGL(glfwGetProcAddress)
		{
			glfwTerminate();
			exit(EXIT_FAILURE);
		}
		glEnable(GL_DEPTH_TEST); // Enabling depth testing allows rear faces of 3D objects to be hidden behind front faces.
		glEnable(GL_MULTISAMPLE); // This call might not be necessary... but this is: "glfwWindowHint(GLFW_SAMPLES, 4)"
		glEnable(GL_BLEND); // GL_BLEND for OpenGL transparency which is further set within the fragment shader. 
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClearColor(0.30f, 0.55f, 0.65f, 1.0f); // Screen clear colour.
	}
};
Source code : C++ from... player_control.h
#pragma once

class Player
{
private:
	const float* axes = nullptr;
	const unsigned char* buttons = nullptr;

	unsigned int camera_position_loc;
	unsigned int view_matrix_loc;

	int num_axes = 0;
	int num_buttons = 0;

	bool joystick_connected = false;
	bool rotate_around_centre = false;

	float screen_aspect_ratio;
	float far_plane = 750.0f;
	float near_plane = 1.0f;
	float FOV = 35.0f;

	float prev_mouse_xpos = 0;
	float prev_mouse_ypos = 0;

	float softening = 0.99f;
	float cam_circling_rate = 1.5f;

	glm::vec3 init_camera_position;
	glm::vec3 init_camera_target;
	glm::vec3 curr_camera_position;
	glm::vec3 curr_camera_target;
	glm::vec3 camera_up;

	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 view_projection;

	glm::mat4 view_dir_mat_xaxis = glm::mat4(1.0f);
	glm::mat4 view_dir_mat_yaxis = glm::mat4(1.0f);
	glm::mat4 rotate_curr_cam_pos = glm::mat4(1.0f);

public:
	bool halt_yaw_tilt = false; // This variable is accessed in: callback_functions.h

	Player(int init_window_width, int init_window_height, Shader& shader)
	{
		int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
		if (present)
			joystick_connection_event(GLFW_JOYSTICK_1, GLFW_CONNECTED);

		init_camera_position = glm::vec3(0.0f, 20.0f, 150.0f); // -Z is into the screen.		
		init_camera_target = init_camera_position;
		init_camera_target.z -= 10; // Any value less than camera Z position value (this also means there's now no need to check the camera view direction with respect to glm::lookAt camera up)									

		curr_camera_position = init_camera_position;
		curr_camera_target = init_camera_target;

		screen_aspect_ratio = (float)init_window_width / (float)init_window_height;

		camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
		view = glm::lookAt(curr_camera_position, curr_camera_target, camera_up);
		projection = glm::perspective(glm::radians(FOV), screen_aspect_ratio, near_plane, far_plane);

		view_projection = projection * view;

		camera_position_loc = glGetUniformLocation(shader.ID, "camera_position");
		glUniform3f(camera_position_loc, curr_camera_position.x, curr_camera_position.y, curr_camera_position.z); // Transfer camera position to fragment shader.		

		view_matrix_loc = glGetUniformLocation(shader.ID, "player_view_matrix");
		glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, glm::value_ptr(view_projection)); // Transfer view matrix to vertex shader.
	}

	void joystick_connection_event(int joy_id, int joy_event)
	{
		if (joy_event == GLFW_CONNECTED)
		{
			std::cout << "\n   GLFW_JOYSTICK_" << joy_id << " Has been connected\n";

			// Which joysticks are detected depends on the PC setup. Also note that a calibration routine (and/or Windows joystick calibration) is required ideally
			// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			if (joy_id == GLFW_JOYSTICK_1)
			{
				axes = glfwGetJoystickAxes(joy_id, &num_axes); // Returns "An array of axis values, or NULL if the joystick is not present or an error occurred"
				if (axes != NULL)
				{
					joystick_connected = true;
					std::cout << "\n   GLFW_JOYSTICK_" << joy_id << " Number of axes: " << num_axes << "\n";
				}
				else
					std::cout << "\n   GLFW_JOYSTICK_" << joy_id << " Connected but an error occurred\n";
			}
			else
				std::cout << "\n   GLFW_JOYSTICK_" << joy_id << " Is not configured\n";
		}
		if (joy_event == GLFW_DISCONNECTED)
		{
			if (joy_id == GLFW_JOYSTICK_1)
				joystick_connected = false;

			std::cout << "\n   GLFW_JOYSTICK_" << joy_id << " Is now disconnected\n";
		}
	}

	void yaw_tilt_mouse_val(double mouse_xpos, double mouse_ypos)
	{
		float cam_yaw_factor = (float)mouse_xpos - prev_mouse_xpos;
		float cam_tilt_factor = (float)mouse_ypos - prev_mouse_ypos;

		prev_mouse_xpos = (float)mouse_xpos;
		prev_mouse_ypos = (float)mouse_ypos;
		// std::cout << "\n   mouse_xpos: " << mouse_xpos << " --- mouse_ypos: " << mouse_ypos << " --- cam_yaw_factor: " << cam_yaw_factor << " --- cam_tilt_factor: " << cam_tilt_factor;

		yaw_tilt_transform(-cam_yaw_factor * 0.035f, -cam_tilt_factor * 0.035f);
	}

	void yaw_tilt_keys_val(GLFWwindow* window, float rate_val) // *** As an example, called from: main() ... this function uses polled key-presses instead of "key_callback" events ***
	{
		float xrate = 0;
		float yrate = 0;

		int ekey = glfwGetKey(window, GLFW_KEY_E);
		int rkey = glfwGetKey(window, GLFW_KEY_R);

		if (ekey == GLFW_PRESS)
			xrate = rate_val;
		if (rkey == GLFW_PRESS)
			xrate = -rate_val;

		int dkey = glfwGetKey(window, GLFW_KEY_D);
		int ckey = glfwGetKey(window, GLFW_KEY_C);

		if (dkey == GLFW_PRESS)
			yrate = rate_val;
		if (ckey == GLFW_PRESS)
			yrate = -rate_val;

		yaw_tilt_transform(xrate, yrate);
	}

	void yaw_tilt_rotate_around_centre(int button, int action, int mods)
	{
		// GLFW_MOUSE_BUTTON_LEFT is defined as 0... GLFW_MOUSE_BUTTON_RIGHT is defined as 1
		// The joystick routine has been set to also pass 0 or 1 for two of the joystick's handle buttons.

		if (button == 0 && action == GLFW_PRESS)
		{
			rotate_around_centre = true;
			rotate_curr_cam_pos = glm::mat4(1.0f);
			rotate_curr_cam_pos = glm::rotate(rotate_curr_cam_pos, glm::radians(cam_circling_rate), glm::vec3(0, 1, 0));
		}
		else if (button == 1 && action == GLFW_PRESS)
		{
			rotate_around_centre = true;
			rotate_curr_cam_pos = glm::mat4(1.0f);
			rotate_curr_cam_pos = glm::rotate(rotate_curr_cam_pos, glm::radians(-cam_circling_rate), glm::vec3(0, 1, 0)); // Negated here.	
		}
		else
			rotate_around_centre = false;

		glUniform3f(camera_position_loc, curr_camera_position.x, curr_camera_position.y, curr_camera_position.z); // This line was added: 03/09/2022... Update camera position (used in fragment shader)
	}

	void camera_zoom(double yoffset)
	{
		glm::vec3 view_direction = curr_camera_target - curr_camera_position;

		if (glm::length(view_direction) > 0) // This check isn't actually required because camera target Z is constantly set to camera position Z - 10   
			view_direction = glm::normalize(view_direction);

		curr_camera_position += view_direction * (float)yoffset * 5.0f; // See: callback_functions.h... yoffset is either -1 or 1 (occasionally 2 or 3)
		curr_camera_target += view_direction * (float)yoffset * 5.0f; // This line was added: 04/08/2022

		glUniform3f(camera_position_loc, curr_camera_position.x, curr_camera_position.y, curr_camera_position.z); // Update camera position (used in fragment shader)		

		view = glm::lookAt(curr_camera_position, curr_camera_target, camera_up);

		view_projection = projection * view;
		glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, glm::value_ptr(view_projection));
	}

	void update_project_matrix(int window_width, int window_height)
	{
		screen_aspect_ratio = (float)window_width / (float)window_height;
		projection = glm::perspective(glm::radians(FOV), screen_aspect_ratio, near_plane, far_plane);

		view_projection = projection * view;
		glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, glm::value_ptr(view_projection));
	}

	void continuously_rotate_view()
	{
		view = glm::rotate(view, 0.0075f, glm::vec3(0, 1, 0));

		view_projection = projection * view;
		glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, glm::value_ptr(view_projection));
	}

private:
	void process_joystick(float& y_val, float& x_val)
	{
		// Temporary Bug workaround (See: joystick_callback() in callback_functions.h)
		// -----------------------------------------------------------------------------------------------------
		axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &num_axes);
		if (axes != NULL)
			joystick_connected = true;
		else
			joystick_connected = false;
		// -----------------------------------
		if (joystick_connected)
		{
			std::cout << "\n   ";
			for (int i = 0; i < num_axes; ++i)
			{
				std::cout << "axes[" << i << "]: " << axes[i] << " --- ";

				if (i == 0) // Check if index exists.
				{
					if (abs(axes[0]) > 0.07f) // Dead-band to stop drift.
						y_val = -axes[0] * 0.65f; // Left & Right for: Logitech Extreme 3D Pro.
					else
						y_val = 0; // Stops dead-band allowing mouse axes to control yaw and tilt whilst joystick is plugged in.
				}
				if (i == 1)
				{
					if (abs(axes[1]) > 0.07f)
						x_val = -axes[1] * 0.65f; // Up & Down.
					else
						x_val = 0; // Comment these two else statements to enable the mouse axes when the joystick is at rest.
				}
			}
			int button_value = -1;
			buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &num_buttons);
			if (buttons != NULL) // Override mouse with joystick.
			{
				for (int i = 0; i < num_buttons; ++i)
				{
					if (i == 2) // Check if index exists.
					{
						if (buttons[2] == GLFW_PRESS) // Left joystick handle button.
						{
							button_value = 0;
							std::cout << "\n   Left mouse button pressed --- buttons[" << i << "]: " << buttons[i];
						}
					}
					if (i == 3)
					{
						if (buttons[3] == GLFW_PRESS) // Right joystick handle button.
						{
							button_value = 1;
							std::cout << "\n   Right mouse button pressed --- buttons[" << i << "]: " << buttons[i];
						}
					}
					/* "For backward compatibility with earlier versions that did not have glfwGetJoystickHats, the button array by default also includes all hats" */
					/*  ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------  */
					if (i == 12) // Check if index exists.	
						if (buttons[12] > 0)
							camera_zoom(0.1); // Joystick hat forwards.

					if (i == 14)
						if (buttons[14] > 0)
							camera_zoom(-0.1); // Joystick hat backwards.	
				}
				if (button_value != -1)
					yaw_tilt_rotate_around_centre(button_value, GLFW_PRESS, -1);
				else
					yaw_tilt_rotate_around_centre(button_value, GLFW_RELEASE, -1);
			}
		}
	}

	void yaw_tilt_transform(float yaxis_val, float xaxis_val)
	{
		float y_val = yaxis_val; // Set as the mouse axis values or keypresses as the default.
		float x_val = xaxis_val;

		process_joystick(y_val, x_val); // Being passed by reference so they can be overridden by the joystick axes.

		if (!halt_yaw_tilt) // Pressing the spacebar disables all movement by the mouse or joystick.
		{
			view_dir_mat_xaxis = glm::rotate(view_dir_mat_xaxis, glm::radians(x_val), glm::vec3(1, 0, 0)); // Axis: 1 of 2) Rotate vertically.

			glm::vec3 view_direction = curr_camera_target - curr_camera_position; // Target - Position is looking into the screen.
			if (glm::length(view_direction) > 0)
				view_direction = glm::normalize(view_direction);

			if (rotate_around_centre)
			{
				if (softening == 0) // Don't start rotating the camera around the scene's centre until the camera has first completed rotating to look directly at the scene's centre.
					curr_camera_position = rotate_curr_cam_pos * glm::vec4(curr_camera_position, 1.0f);

				// "view_direction" is declared just before this if-statement to allow its values to be displayed in the console window (as done further down) without it being centred in here 1st.

				// glm::vec3 view_direction = glm::normalize(curr_camera_target - curr_camera_position); // Target - Position is looking into the screen.
				// std::cout << "\n   view_direction.x: " << view_direction.x << " --- view_direction.y: " << view_direction.y << " --- view_direction.z: " << view_direction.z;					

				glm::vec3 view_dir_proj_XZ = glm::vec3(view_direction.x, 0.0f, view_direction.z);
				if (glm::length(view_dir_proj_XZ) > 0)
					view_dir_proj_XZ = glm::normalize(view_dir_proj_XZ);

				glm::vec3 cam_to_centre_dir_proj_XZ = -glm::vec3(curr_camera_position.x, 0.0f, curr_camera_position.z);
				if (glm::length(cam_to_centre_dir_proj_XZ) > 0)
					cam_to_centre_dir_proj_XZ = glm::normalize(cam_to_centre_dir_proj_XZ);

				float dot_product = glm::dot(view_dir_proj_XZ, cam_to_centre_dir_proj_XZ);
				float dot_prod_centre = glm::dot(glm::vec3(0, 0, -1), cam_to_centre_dir_proj_XZ);

				// ACOS Warning... Don't call glm::acos on values below -1 or above +1 (even if theoretically exactly -1 or +1, the value could easily end up at e.g. 1.000001 due to precision limitations)
				// ------------------------------------------------------------------------------------------
				if (dot_product < -1.0f)
					dot_product = -1.0f;
				if (dot_product > 1.0f)
					dot_product = 1.0f;

				if (dot_prod_centre < -1.0f)
					dot_prod_centre = -1.0f;
				if (dot_prod_centre > 1.0f)
					dot_prod_centre = 1.0f;

				float view_dir_spin_angle_deg = glm::acos(dot_product) * 57.2958f;
				float view_dir_spin_angle_centred = glm::acos(dot_prod_centre) * 57.2958f;

				// std::cout << "\n   view_dir_spin_angle_deg: " << view_dir_spin_angle_deg << " --- dot_product: " << dot_product << " --- curr_camera_position.z: " << curr_camera_position.z << " --- view_direction.x: " << view_direction.x;
				// std::cout << "\n   view_dir_spin_angle_centred: " << view_dir_spin_angle_centred << " --- dot_prod_centre: " << dot_prod_centre;

				if (curr_camera_position.x < 0) // Set to rotate to the scene's centred Z-axis in the opposite direction if the camera is in the left half of the scene.
					view_dir_spin_angle_centred = -view_dir_spin_angle_centred;

				glm::mat4 rotate_to_centre(1.0f);
				rotate_to_centre = glm::rotate(rotate_to_centre, glm::radians(-view_dir_spin_angle_centred), glm::vec3(0, 1, 0));

				glm::vec3 view_dir_proj_XZ_centred = glm::vec3(rotate_to_centre * glm::vec4(view_dir_proj_XZ, 1.0f));
				if (glm::length(view_dir_proj_XZ_centred) > 0)
					view_dir_proj_XZ_centred = glm::normalize(view_dir_proj_XZ_centred);

				// std::cout << "\n   view_dir_proj_XZ_centred.x: " << view_dir_proj_XZ_centred.x;

				if (view_dir_proj_XZ_centred.x < 0) // If the camera is looking to the left then rotate the camera clockwise to look directly at the scene's centre, otherwise anticlockwise.
					view_dir_spin_angle_deg = -view_dir_spin_angle_deg;

				softening *= softening;
				if (softening < 0.05f || view_dir_spin_angle_deg == 0) // Apply gradually the rotation of the camera when rotating to look directly at the scene's centre.
					softening = 0;

				float rotation_factor = 1 - softening;
				view_dir_mat_yaxis = glm::rotate(view_dir_mat_yaxis, glm::radians(view_dir_spin_angle_deg * rotation_factor), glm::vec3(0, 1, 0)); // Axis: 2 of 2) Rotate horizontally.			
			}
			else
			{
				softening = 0.99f;
				view_dir_mat_yaxis = glm::rotate(view_dir_mat_yaxis, glm::radians(y_val), glm::vec3(0, 1, 0)); // Axis: 2 of 2) Rotate horizontally.
			}
			// std::cout << "\n   view_direction.x: " << view_direction.x << " --- view_direction.y: " << view_direction.y << " --- view_direction.z: " << view_direction.z;

			curr_camera_target = curr_camera_position; // Target effectively gets reset here, and therefore its rotation is restored by "target_relative_to_camera_at_zero" below.
			curr_camera_target.z -= 10; // Any value less than camera Z position value (this also means there's now no need to check the camera view direction with respect to glm::lookAt camera up)

			glm::mat4 target_relative_to_camera_at_zero(1.0f); // Rotates the target around the camera's current position.

			// Note: the Y axis must be applied globally (to the left of *) to the X axis, or else the orientation can become totally crooked after sufficient camera position change
			// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			target_relative_to_camera_at_zero = glm::translate(target_relative_to_camera_at_zero, curr_camera_position);
			target_relative_to_camera_at_zero = target_relative_to_camera_at_zero * view_dir_mat_yaxis * view_dir_mat_xaxis; // Apply yaxis globally to xaxis (xaxis acts locally)
			target_relative_to_camera_at_zero = glm::translate(target_relative_to_camera_at_zero, -curr_camera_position);

			curr_camera_target = target_relative_to_camera_at_zero * glm::vec4(curr_camera_target, 1.0f);
			view = glm::lookAt(curr_camera_position, curr_camera_target, camera_up);

			view_projection = projection * view;
			glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, glm::value_ptr(view_projection));
		}
	}
};
Source code : C++ from... callback_functions.h
#pragma once

// Set GLFW Callback Function Pointer (See 1st answer) https://gamedev.stackexchange.com/questions/71721/how-can-i-forward-glfws-keyboard-input-to-another-object

class Callback_Functions // Set in main: "Callback_Functions callback_state(window_params, player, light_source_0)"
{
public:
	Callback_Functions(Player& player, Shadow& light_source_0, Window_Params& window_params) : player(player), light_source(light_source_0), window_params(window_params)
	{
		glfwSetKeyCallback(window_params.window, key_callback); // https://www.glfw.org/docs/3.3/input_guide.html
		glfwSetCharCallback(window_params.window, character_callback);
		glfwSetMouseButtonCallback(window_params.window, mouse_button_callback);
		glfwSetScrollCallback(window_params.window, scroll_callback);
		glfwSetCursorPosCallback(window_params.window, cursor_position_callback);
		glfwSetCursorEnterCallback(window_params.window, cursor_enter_callback);
		glfwSetJoystickCallback(joystick_callback);
		// ---------------------------------------------------
		glfwSetWindowSizeCallback(window_params.window, window_size_callback); // https://www.glfw.org/docs/3.3/window_guide.html
		glfwSetFramebufferSizeCallback(window_params.window, framebuffer_size_callback);
		glfwSetWindowPosCallback(window_params.window, window_pos_callback);
		glfwSetWindowMaximizeCallback(window_params.window, window_maximize_callback);
	}

private:
	Player& player;
	Shadow& light_source;
	Window_Params& window_params;

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		// Set in: main() "glfwSetWindowUserPointer(window_params.window, &callback_state)"
		// ------------------------------------------------------------------------------------------------------------------
		Callback_Functions* state = static_cast<Callback_Functions*>(glfwGetWindowUserPointer(window));

		// RELEASE events are currently not being used here.	
		// Values can be set directly in here within the callback functions, or by calling some player class function as done further down.

		if (key == GLFW_KEY_Q)
			if (action == GLFW_PRESS || action == GLFW_REPEAT) // Light's vertical position value.
				state->light_source.light_pos.y += 0.45f;

		if (key == GLFW_KEY_A)
			if (action == GLFW_PRESS || action == GLFW_REPEAT)
				state->light_source.light_pos.y -= 0.45f;

		if (key == GLFW_KEY_X)
			if (action == GLFW_PRESS || action == GLFW_REPEAT)
			{
				glm::mat4 rotate_light(1.0f);
				rotate_light = glm::rotate(rotate_light, glm::radians(5.0f), glm::vec3(0, 1, 0));
				state->light_source.light_pos = rotate_light * glm::vec4(state->light_source.light_pos, 1.0f);
			}
		if (key == GLFW_KEY_Z)
			if (action == GLFW_PRESS || action == GLFW_REPEAT)
			{
				glm::mat4 rotate_light(1.0f);
				rotate_light = glm::rotate(rotate_light, glm::radians(-5.0f), glm::vec3(0, 1, 0));
				state->light_source.light_pos = rotate_light * glm::vec4(state->light_source.light_pos, 1.0f);
			}
		if (key == GLFW_KEY_G)
			if (action == GLFW_PRESS || action == GLFW_REPEAT) // Light's far-plane position value.
				state->light_source.far_plane -= 0.5f;

		if (key == GLFW_KEY_H)
			if (action == GLFW_PRESS || action == GLFW_REPEAT)
				state->light_source.far_plane += 0.5f;

		if (key == GLFW_KEY_V)
			if (action == GLFW_PRESS || action == GLFW_REPEAT) // Light's near-plane position value.
				state->light_source.near_plane -= 0.5f;

		if (key == GLFW_KEY_B)
			if (action == GLFW_PRESS || action == GLFW_REPEAT)
				state->light_source.near_plane += 0.5f;

		if (key == GLFW_KEY_1)
			if (action == GLFW_PRESS || action == GLFW_REPEAT) // Light's FOV value.
				state->light_source.light_FOV -= 0.75f;

		if (key == GLFW_KEY_2)
			if (action == GLFW_PRESS || action == GLFW_REPEAT)
				state->light_source.light_FOV += 0.75f;

		if (key == GLFW_KEY_SPACE)
		{
			if (action == GLFW_PRESS || action == GLFW_REPEAT)
				state->player.halt_yaw_tilt = true;
			else
				state->player.halt_yaw_tilt = false;
		}

		if (key == GLFW_KEY_F && action == GLFW_PRESS)
		{
			state->window_params.in_fullscreen_mode = true;

			glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), NULL, NULL, state->window_params.monitor_width,
				state->window_params.monitor_height, state->window_params.mode->refreshRate);
			glfwSwapInterval(1); // Set VSync rate 1:1 with monitor's refresh rate.
		}
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			state->window_params.in_fullscreen_mode = false;

			glfwSetWindowMonitor(window, NULL, state->window_params.width_offset, state->window_params.height_offset,
				state->window_params.curr_window_width, state->window_params.curr_window_height, state->window_params.mode->refreshRate);
			glfwSwapInterval(1);
		}
	}

	static void character_callback(GLFWwindow* window, unsigned int codepoint)
	{

	}

	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{
		Callback_Functions* state = static_cast<Callback_Functions*>(glfwGetWindowUserPointer(window));
		state->player.yaw_tilt_rotate_around_centre(button, action, mods);
	}

	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		Callback_Functions* state = static_cast<Callback_Functions*>(glfwGetWindowUserPointer(window));
		state->player.camera_zoom(yoffset); // Offset values are either -1 or 1 (occasionally +/- 2 or 3)			
	}

	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
		Callback_Functions* state = static_cast<Callback_Functions*>(glfwGetWindowUserPointer(window));
		state->player.yaw_tilt_mouse_val(xpos, ypos);
	}

	static void cursor_enter_callback(GLFWwindow* window, int entered)
	{
		std::cout << "\n\n   Cursor has entred or left the window's content area\n";
	}

	static void joystick_callback(int joy_id, int joy_event)
	{
		Callback_Functions* state = static_cast<Callback_Functions*>(glfwGetJoystickUserPointer(joy_id)); // https://github.com/glfw/glfw/issues/2092 (BUG!)
		if (state)
			state->player.joystick_connection_event(joy_id, joy_event);
		else
			std::cout << "\n   glfwGetJoystickUserPointer() has returned a null pointer\n";
	}

	static void window_size_callback(GLFWwindow* window, int width, int height)
	{
		Callback_Functions* state = static_cast<Callback_Functions*>(glfwGetWindowUserPointer(window));
		state->player.update_project_matrix(width, height); // Width and height are used for the screen aspect ratio.

		if (!state->window_params.in_fullscreen_mode)
		{
			state->window_params.curr_window_width = width; // Used to set the window to the last known window size when exiting full-screen mode.
			state->window_params.curr_window_height = height;
		}
	}

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		Callback_Functions* state = static_cast<Callback_Functions*>(glfwGetWindowUserPointer(window));
		state->window_params.frame_buffer_width = width; // These are passed from the main-loop to: glViewport() within the draw-call functions within: load_meshes_binary.h
		state->window_params.frame_buffer_height = height;
	}

	static void window_pos_callback(GLFWwindow* window, int xpos, int ypos)
	{
		Callback_Functions* state = static_cast<Callback_Functions*>(glfwGetWindowUserPointer(window));

		if (!state->window_params.in_fullscreen_mode) // Note that this callback's xpos & ypos are: window upper-left position values.	
			glfwGetWindowPos(window, &state->window_params.width_offset, &state->window_params.height_offset); // Set by reference: upper-left offset values.		
	}

	static void window_maximize_callback(GLFWwindow* window, int maximized)
	{
		std::cout << "\n   Window size has been maximized";
	}
};

Source code : GLSL from... shader_glsl.vert(Vertex shader)
#version 420 core

layout(location = 0) in vec3 aPos;	 // Attribute data: vertex(s) X, Y, Z position via VBO set up on the CPU side.
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in unsigned int aMeshNum; // Use this for identifying and transforming meshes independently.
layout(location = 4) in unsigned int aSamplerPos; // Pass this to the fragment shader.

out vec3 vert_pos_varying; // Vertex position coordinates passed to the fragment shader as interpolated per-vertex.
out vec3 vert_pos_transformed; // Transformed cube vertex position coordinates also passed as interpolated.
out vec3 vertex_normal;
out vec2 texture_coordinates;

out vec4 light_view_vert_pos[10]; // Transform the vertex position for each light view and pass to the fragment shader. 
uniform mat4 light_view_matrix[10]; // This is: projection * view.
uniform unsigned int light_count;
uniform unsigned int light_ID;

flat out unsigned int mesh_number; // Can be used to identify and transform meshes independently of one another.
flat out unsigned int sampler_array_pos;

uniform unsigned int model_number;
uniform bool rendering_shadow_map;
uniform int rendering_multiple_meshes;

uniform mat4 camera_far_plane[10];
uniform mat4 camera_near_plane[10];
uniform mat4 transform_light_pos_mat[10]; // Used only during the specific draw call for the light model (or its frustum) being rendered.
uniform mat4 spin_on_the_spot_mat;

uniform mat4 player_view_matrix; // This is: projection * view.

void main()
{
	if (rendering_multiple_meshes == -1)
		mesh_number = aMeshNum; // Receive mesh number via input attribute.
	else
		mesh_number = rendering_multiple_meshes; // For draw method option 0... Receive mesh number via uniform.

	mat4 model_matrix = mat4(1.0);

	if (model_number == 0)
		model_matrix = spin_on_the_spot_mat;

	if (model_number == 1) // Main scene... do nothing.
	{

	}
	if (model_number == 2 || model_number == 3) // Lights.
	{
		model_matrix = transform_light_pos_mat[light_ID];

		if (mesh_number == 0)
			model_matrix = model_matrix * camera_far_plane[light_ID]; // Move along local axis.

		if (mesh_number == 3)
			model_matrix = model_matrix * camera_near_plane[light_ID];
	}

	if (model_number == 4)
		model_matrix = transform_light_pos_mat[light_ID];

	mat3 normal_matrix = transpose(inverse(mat3(model_matrix)));
	vertex_normal = normal_matrix * aNormal;

	if (length(vertex_normal) > 0)
		vertex_normal = normalize(vertex_normal); // Never try to normalise zero vectors (0,0,0)

	vert_pos_varying = aPos; // Send aPos vertex position values to fragment shader, which can be used as colour values instead of using texture images.
	vert_pos_transformed = vec3(model_matrix * vec4(aPos, 1.0)); // Send transformed position values, which are used for the lighting effects.

	if (rendering_shadow_map)
		gl_Position = light_view_matrix[light_ID] * model_matrix * vec4(aPos, 1.0); // Render shadow map.
	else
	{
		texture_coordinates = aTexCoord;
		sampler_array_pos = aSamplerPos;

		for (unsigned int i = 0; i < light_count; ++i)
			light_view_vert_pos[i] = light_view_matrix[i] * model_matrix * vec4(aPos, 1.0);

		gl_Position = player_view_matrix * model_matrix * vec4(aPos, 1.0); // Output to vertex stream for the "Vertex Post-Processing" stage.	
	}
}
Source code : GLSL from... shader_glsl.frag(Fragment shader)
#version 420 core

out vec4 fragment_colour;

// Must be the exact same name as declared in the vertex shader
// -----------------------------------------------------------------------------------
in vec3 vert_pos_varying; // Vertex position coordinates received from the fragment shader as interpolated per-vertex.
in vec3 vert_pos_transformed; // Transformed cube vertex position coordinates also received as interpolated.
in vec3 vertex_normal;
in vec2 texture_coordinates;

flat in unsigned int mesh_number; // Can be used to set the fragment colours of meshes independently of one another.
flat in unsigned int sampler_array_pos; // Used to select the correct image from the images[] array.

in vec4 light_view_vert_pos[10];
uniform vec3 light_position[10];
uniform sampler2D shadow_map[10];
uniform unsigned int light_count;
uniform unsigned int light_ID;

uniform sampler2D images[32]; // GL_MAX_TEXTURE_IMAGE_UNITS = 32 on my PC.

uniform unsigned int model_number;
uniform bool rendering_shadow_map;
uniform bool meshes_combined;

uniform vec3 camera_position; // Players view position which is set in: player_control.h

void main()
{
	if (rendering_shadow_map)
	{
		if (model_number == 4) // Don't render the frustum during the shadow map run.
			discard;

		if (model_number == 2 || model_number == 3) // Lights 1 and 2... likewise don't allow near and far planes to cast shadows.
			if (mesh_number == 0 || mesh_number == 3)
				discard;

		// gl_FragDepth = gl_FragCoord.z; // This effectively happens automatically within the graphics pipeline, so no need to uncomment.
	}
	else
	{
		vec3 view_direction = normalize(camera_position - vert_pos_transformed);

		unsigned int index = 0;
		// --------------------------
		if (meshes_combined)
			index = sampler_array_pos;

		vec4 image_colour = texture(images[index], texture_coordinates);

		float pcf_depth;

		for (unsigned int i = 0; i < light_count; ++i)
		{
			if (model_number == 4) // Rendering the frustum.
			{
				if (light_ID == 0 && i == 1) // For light 0... exit the for-loop or else processing light 1 crops light 0's frustum.
					break;
				if (light_ID == 1 && i == 0) // Likewise... ignore the loop 1st iteration or else light 0 crops light 1's frustum.
					continue;
			}
			vec3 vec_pos_ndc = light_view_vert_pos[i].xyz / light_view_vert_pos[i].w; // For an orthographic projection matrix the: / w... simply has no effect because it remains at 1.  

			// Self shadowing... Note that the terms "shadow acne" and "erroneous self-shadowing" are used synonymously
			// -------------------------------------------------------------------------------------------------------------------------------------------------			
			// Ideally the bias values set below should change dynamically based on various parameters. 		
			// Additional techniques can also be used. For example: calculating a tight projection, cascading shadow maps, and more.
			// Particularly the light's near-plane distance should be kept as close as possible to the objects casting shadows, but also the far-plane.
			// https://developer.nvidia.com/content/depth-precision-visualized (Includes graphs to help explain the nature of the... 1/z depth buffer)
			// https://stackoverflow.com/questions/60195019/why-is-the-z-coordinate-flipped-after-multiplying-with-a-matrix-in-glsl-opengl (Now Z+ is into the screen)
			vec3 converted_coords = vec_pos_ndc * 0.5 + 0.5; // Convert XY [-1, 1] NDC to [0, 1] Texture... also Z [-1, 1] NDC to [0, 1] Depth buffer.																			

			vec3 light_direction = normalize((light_position[i] - vert_pos_transformed)); // A position used as a light source acts as a point light (Not a directional light)		
			float bias = max(0.025 * (1.0 - dot(vertex_normal, light_direction)), 0.005); // Slope scale depth bias (original values were set to: 0.025 and 0.005)				
				// bias = 0; // Make sure this is commented (use for testing purposes only)

			vec2 texel_size = 1.0 / textureSize(shadow_map[i], 0);

			float shadow_texels = 0;
			int steps = 1; // Set PCF shadow softening amount.
			for (int x = -steps; x <= steps; ++x)
			{
				for (int y = -steps; y <= steps; ++y)
				{
					pcf_depth = texture(shadow_map[i], converted_coords.xy + (vec2(x, y) * texel_size)).r;
					shadow_texels += converted_coords.z - bias > pcf_depth ? 1.0 : 0.0; // Depth buffer is +Z into screen (see 2nd post in the link at the bottom for explanation about camera Z direction)

					// Restrict shadows to inside the frustum
					// ---------------------------------------------------							
					// if (vec_pos_ndc.x > 1 || vec_pos_ndc.x < -1 || vec_pos_ndc.y > 1 || vec_pos_ndc.y < -1 || vec_pos_ndc.z > 1 || vec_pos_ndc.z < -1)
						// shadow_texels = 0.0; // Set above 0 (above 0.5 starts to become noticeable depending on "steps" value) to cast shadows everywhere apart from inside the frustum.			
				}
			}
			// Shadow factor range: [0 = no shadow, 1 = full shadow] E.G. steps = 2... for-loops: 5 iterations each = 25 / ( ((2 * 2) + 1) * ((2 * 2) + 1) = 25 ) = 1.0
			// --------------------------------------------------------------------------------------------
			float shadow_factor = shadow_texels / (((steps * 2) + 1) * ((steps * 2) + 1));
			float light_percent = 0.25; // Decrease this value according to how many lights there are... or even use: "model_number" to set each light's brightness individually.
			float lighting_multiplier = light_percent - (shadow_factor * 0.20);

			// Comment to cast shadows beyond near and far planes (Only effective if the above "Restrict shadows to inside the frustum" is commented)
			// ------------------------------------------------------------------------
			if (converted_coords.z > 1.0)
				lighting_multiplier = light_percent;

			vec4 ambient_result = vec4(lighting_multiplier * image_colour.rgb, 1.0);

			float diffuse_angle = max(dot(light_direction, vertex_normal), -0.05); // [-1.0 to 0] down to -1 results in darker lighting past 90 degrees.
			vec4 diffuse_result = vec4(lighting_multiplier * diffuse_angle * image_colour.rgb, 1.0);

			vec3 specular_colour = vec3(0.65, 0.65, 0.65);
			vec3 reflect_direction = normalize(reflect(-light_direction, vertex_normal)); // Light direction is negated here.
			float specular_strength = pow(max(dot(view_direction, reflect_direction), 0), 64);
			vec4 specular_result = vec4(lighting_multiplier * specular_colour * specular_strength, 1.0);

			// Incrementally add each light's lighting and shadowing effect to "fragment_colour"
			// -----------------------------------------------------------------------------------------------------------
			// Only apply lighting and shadowing to within the frustums
			// ----------------------------------------------------------------------------
			if (vec_pos_ndc.x > 1 || vec_pos_ndc.x < -1 || vec_pos_ndc.y > 1 || vec_pos_ndc.y < -1 || vec_pos_ndc.z > 1 || vec_pos_ndc.z < -1)
				fragment_colour += ambient_result; // Disable this if-statement to disable this restriction.	
			else
				fragment_colour += ambient_result + diffuse_result + specular_result; // Always leave this line enabled.

			if (model_number == 2 || model_number == 3) // Light 1 and light 2
			{
				if (mesh_number == 0 || mesh_number == 3) // Set near and far-plane colour tints and transparency.
				{
					// discard; // Uncomment to not draw the planes.

					if (light_ID == 0)
						fragment_colour.r *= 10;
					if (light_ID == 1)
						fragment_colour.b *= 10;

					fragment_colour.a = 0.25;
				}
				if (mesh_number == 2)
					fragment_colour = vec4(0.85, 0.85, 0.85, 1.0); // Set shadow camera front face to a solid colour.
			}
			if (model_number == 1 && mesh_number == 0) // Main scene floor.
			{
				// Note: for 2 lights (or more) ... the code requires modifying if wanting to display both frustums Z colour values simultaneously (currently the last light overrides all previous for-loop iterations) 

				// if ((vec_pos_ndc.z > 0.90 && vec_pos_ndc.z <= 1.001) && pcf_depth != 1.0) // pcf_depth != 1.0 restricts it to inside the frustum.
					// fragment_colour = lighting_multiplier * (1 / light_percent) * vec4(1.0, 0.2, 0.2, 1.0);

				// if (vec_pos_ndc.z <= 0.90 && vec_pos_ndc.z > 0.80 && pcf_depth != 1.0)
					// fragment_colour = lighting_multiplier * (1 / light_percent) * vec4(0.70, 0.25, 0.15, 1.0);

				// if (vec_pos_ndc.z <= 0.80 && vec_pos_ndc.z > 0.70 && pcf_depth != 1.0)
					// fragment_colour = lighting_multiplier * (1 / light_percent) * vec4(0.5, 0.45, 0.10, 1.0);

				// if (vec_pos_ndc.z <= 0.70 && vec_pos_ndc.z > 0.60 && pcf_depth != 1.0)
					//  fragment_colour = lighting_multiplier * (1 / light_percent) * vec4(0.3, 0.65, 0.05, 1.0);

				// if (vec_pos_ndc.z <= 0.60 && vec_pos_ndc.z > 0.50 && pcf_depth != 1.0)
					// fragment_colour =lighting_multiplier * (1 / light_percent) * vec4(0.1, 0.95, 0.0, 1.0);

				// ---------------------------------------------------------------------------

				// if (vec_pos_ndc.x > 0.985 && vec_pos_ndc.x < 0.999 && pcf_depth != 1.0) // Simply adds narrow strips to the sides of the frustum. 
					// fragment_colour = vec4(0.15, 0.25, 0.95, 1.0);

				// if (vec_pos_ndc.x < -0.985 && vec_pos_ndc.x > -0.999 && pcf_depth != 1.0)
					// fragment_colour = vec4(0.15, 0.25, 0.95, 1.0);				
			}
			if (model_number == 4) // Frustum.
			{
				discard; // Uncomment to not draw the frustums.

					// if (vec_pos_ndc.x > 1 || vec_pos_ndc.x < -1 || vec_pos_ndc.y > 1 || vec_pos_ndc.y < -1 ||vec_pos_ndc.z > 1) // Works for Perspective.
				if (vec_pos_ndc.x > 1 || vec_pos_ndc.x < -1 || vec_pos_ndc.y > 1 || vec_pos_ndc.y < -1 || vec_pos_ndc.z > 1 || vec_pos_ndc.z < -1) // Works for Orthographic and Perspective.
					discard;

				if (light_ID == 0)
					fragment_colour = vec4(1.0, 0.0, 0.0, 0.025); // Transparent red.
				if (light_ID == 1)
					fragment_colour = vec4(0.0, 0.0, 1.0, 0.025); // Transparent blue.
			}
		}
	}
}