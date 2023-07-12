#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "../include/GL/Time.h"
#include "../include/GL/VBO.h"
#include "../include/GL/VAO.h"
#include "../include/GL/Shader.h"

#include <iostream>
#include <iomanip>

#include "GameOfLife.h"

enum GameState {
	RUNNING,
	PAUSED
};

class GameTime {
private:
	double total_time = 0;
	double pause_start_time = 0;
	double pause_end_time = 0;
	double pause_total_time = 0;
public:
	GameTime() = default;
	void Update() {
		total_time = Time::time;
	}
	double GetGameTime() const {
		return total_time-pause_total_time;
	}
	void Pause(const bool pause) {
		if(pause) {
			pause_start_time = Time::time;
		} else {
			pause_end_time = Time::time;
			pause_total_time = pause_end_time - pause_start_time;
		}
	}
};

class GameOfLifeView {
private:
	GameState state = RUNNING;
	const float quad_vertices_ndc[12] = {
		-1.0f, -1.0f,
		-1.0f, 1.0f,
		1.0f, 1.0f,
		-1.0f, -1.0f,
		1.0f, 1.0f,
		1.0f, -1.0f
	};
	const float quad_vertices[12] = {
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};
	const float line_vertices[4] = {
		0.0f, 0.0f,
		1.0f,0.0f
	};
	//GLuint canvas_vbo=0;
	//GLuint canvas_vao=0;

	GLuint canvas_line_vbo=0;
	GLuint canvas_line_vao=0;

	GLuint cells_vbo=0;
	GLuint cells_instances_offsets_vbo=0;
	GLuint cells_vao=0;

	float cell_size = 10.0f;
	glm::vec3 cell_color = {1.0f,1.0f,1.0f};

	//glm::vec2 canvas_offset = {cell_size/2.0f,cell_size/2.0f};
	float canvas_line_thickness = 1.0f;
	glm::vec3 canvas_line_color = {0.0f,0.0f,0.0f};

	const double default_tick_timeout = 0.15;
	double tick_timeout = default_tick_timeout; //in seconds
	double tick_timeout_multiplier = 1.0;
	double tick_timeout_multiplier_inc = 0.1;
	double last_tick_time = 0.0;

	unsigned long long tick_count = 0;

	void remapCellsVBO() {
		VBO::bind(cells_instances_offsets_vbo);
		std::vector<GameOfLife::Cell> cells_vector(game_of_life.alive_cells.begin(),game_of_life.alive_cells.end());
		VBO::setData(cells_instances_offsets_vbo,cells_vector.size()*sizeof(GameOfLife::Cell),cells_vector.data(),GL_STATIC_DRAW);
		VBO::unbind();
	}
	void tick() {
		//std::cout<<"GAME: Tick "<<tick_count<<" at "<<std::fixed<<std::setprecision(2)<<Time::time<<" sec\n";
		tick_count+=1;
		game_of_life.Tick();
		remapCellsVBO();
	}
	GameOfLife game_of_life;
	glm::vec2 viewpoint_size = {800,600};
	glm::ivec2 canvas_line_count = {0,0};
	void recalculateCanvasLineCountPos() {
		zoom_offset = glm::vec2(viewpoint_size/zoom_factor-viewpoint_size)/2.0f;
		canvas_line_count = glm::ivec2(glm::vec2(viewpoint_size.y+zoom_offset.y*2,viewpoint_size.x+zoom_offset.x*2)/cell_size)+2;
	}
	void initGL() {
		//VBO::generate(canvas_vbo,sizeof(quad_vertices_ndc),&quad_vertices_ndc,GL_STATIC_DRAW);
		//VBO::bind(canvas_vbo);
		//VAO::generate(canvas_vao);
		//VAO::addAttrib(canvas_vao,0,2,GL_FLOAT,GL_FALSE,2*sizeof(float),(void*)0);
		//VAO::unbind();

		VBO::generate(canvas_line_vbo,sizeof(quad_vertices),&quad_vertices,GL_STATIC_DRAW);
		VBO::bind(canvas_line_vbo);
		VAO::generate(canvas_line_vao);
		VAO::addAttrib(canvas_line_vao,0,2,GL_FLOAT,GL_FALSE,2*sizeof(float),(void*)0);
		VAO::unbind();

		VBO::generate(cells_vbo,sizeof(quad_vertices),&quad_vertices,GL_STATIC_DRAW);
		VBO::bind(cells_vbo);
		VAO::generate(cells_vao);
		VAO::addAttrib(cells_vao,0,2,GL_FLOAT,GL_FALSE,2*sizeof(float),(void*)0);
		VBO::generate(cells_instances_offsets_vbo,0,0,GL_STATIC_DRAW);
		VBO::bind(cells_instances_offsets_vbo);
		VAO::addAttrib(cells_vao,1,2,GL_FLOAT,GL_FALSE,2*sizeof(float),(void*)0);
		VAO::setAttribDivisor(cells_vao,1,1);
		VAO::unbind();
	}
	float zoom_factor = 1.0f;
	float min_zoom = 0.2f;
	glm::vec2 zoom_offset = {0.0f,0.0f};
	GameTime game_timeime;
public:
	double GetTickTimeout() const {
		return tick_timeout;
	}
	void SetTickTimeout(const double timeout) {
		tick_timeout = timeout;
	}
	void SpeedUpTicks() {
		tick_timeout_multiplier-=tick_timeout_multiplier_inc;
	}
	void SpeedDownTicks() {
		tick_timeout_multiplier+=tick_timeout_multiplier_inc;

	}
	glm::vec2 offset = {0.0f,0.0f};
	void SetZoomFactor(const float zoom) {
		zoom_factor = zoom;
		if(zoom_factor<min_zoom) {
			zoom_factor=min_zoom;
		}
		recalculateCanvasLineCountPos();
	}
	float GetZoomFactor() const {
		return zoom_factor;
	}
	void SetViewpointSize(const float width, const float height) {
		viewpoint_size.x = width;
		viewpoint_size.y = height;
		recalculateCanvasLineCountPos();
	}
	void Pause(const bool pause) {
		game_timeime.Pause(pause);
		if(pause) {
			state = PAUSED;
			std::cout<<"GAME: Paused\n";
		}else {
			state = RUNNING;
			std::cout<<"GAME: Unpaused\n";
		}
	}
	bool IsPaused() const {
		if(state == PAUSED) {
			return true;
		}
		return false;
	}
	void SetCells(const std::vector<GameOfLife::Cell>& cells) {
		game_of_life.alive_cells = {cells.begin(),cells.end()};
		remapCellsVBO();
	}
	void Init() {
		initGL();
	}
	void Init(const float screen_x,const float screen_y) {
		SetViewpointSize(screen_x,screen_y);
		initGL();
	}
	void Update() {
		game_timeime.Update();
		if(state == PAUSED) {
			return;
		}
		if(game_timeime.GetGameTime()-last_tick_time>=tick_timeout*tick_timeout_multiplier){
			tick();
			last_tick_time = game_timeime.GetGameTime();
		}
	}
	void Draw(const Shader& cellShader,const Shader& canvasLineShader) const {
		//draw cells
		VAO::bind(cells_vao);
		cellShader.use();
		cellShader.setVec3("color",cell_color);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model,glm::vec3(offset,0.0f));
		model = glm::scale(model,glm::vec3(cell_size));
		cellShader.setMat4("model",model);
		glDrawArraysInstanced(GL_TRIANGLES,0,6,game_of_life.alive_cells.size());

		//draw canvas (lines)
		VAO::bind(canvas_line_vao);
		canvasLineShader.use();
		canvasLineShader.setFloat("cellSize",cell_size);
		canvasLineShader.setVec3("color",canvas_line_color);
		glm::mat4 cellTransformedModel = glm::mat4(1.0f);
		cellTransformedModel = glm::translate(cellTransformedModel,glm::vec3(glm::mod(offset+zoom_offset,cell_size)-glm::vec2(cell_size)-zoom_offset,0.0f));
		model = glm::scale(cellTransformedModel,glm::vec3(viewpoint_size.x/zoom_factor+cell_size,canvas_line_thickness,1.0f));
		canvasLineShader.setMat4("model",model);
		canvasLineShader.setIVec2("lineInstanceOffsetSide",0,1);
		glEnable(GL_POLYGON_SMOOTH);
		//glBlendFunc(GL_SRC_ALPHA,);
		glDrawArraysInstanced(GL_TRIANGLES,0,6,canvas_line_count.x);
		model = glm::scale(cellTransformedModel,glm::vec3(canvas_line_thickness,viewpoint_size.y/zoom_factor+cell_size,1.0f));
		canvasLineShader.setMat4("model",model);
		canvasLineShader.setIVec2("lineInstanceOffsetSide",1,0);
		glDrawArraysInstanced(GL_TRIANGLES,0,6,canvas_line_count.y);
		glDisable(GL_POLYGON_SMOOTH);
		//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);


		//VAO::bind(canvas_vao);
		//canvasShader.use();
		//canvasShader.setFloat("cellSize",cell_size);
		//canvasShader.setFloat("scale",scale);
		//canvasShader.setVec2("canvasOffset",offset);
		//canvasShader.setFloat("lineThickness",canvas_line_thickness);
		//canvasShader.setVec3("lineColor",canvas_line_color);
		//glDrawArrays(GL_TRIANGLES,0,6);
	}
};