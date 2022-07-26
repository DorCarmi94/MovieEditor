#include "Project.h"
#include <iostream>
#include <igl/get_seconds.h>
#include<windows.h>



static void printMat(const Eigen::Matrix4d & mat)
{
	std::cout << " matrix:" << std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout << mat(j, i) << " ";
		std::cout << std::endl;
	}
}

Project::Project()
{
	objectIdxToCameraIdx = std::map<int, int>();
}


void Project::Init()
{
	AddTexture("../../textures/cubemaps/Daylight Box_", 3);
	AddTexture("../../textures/box0.bmp", 2);
	AddTexture("../../textures/plane.png", 2);
	AddTexture("../../textures/camera.png", 2);


	AddShader("../../shaders/pickingShader");
	AddShader("../../shaders/cubemapShader");
	AddShader("../../shaders/basicShader");
	AddShader("../../shaders/basicShaderCopy");


	for (int i = 0; i < 4; i++) {
		AddMaterial(texIDs + i, slots + i, 1);
	}

	//skybox start
	AddShape(Cube, -2, TRIANGLES);	
	SetShapeShader(data_list.size()-1, 1);
	SetShapeMaterial(data_list.size() - 1, 0);
	selected_data_index = 0;
	float s = 100;
	ShapeTransformation(scaleAll, s, 0);
	SetShapeStatic(0);
	//skybox end
	
	//bezier start
	AddShape(Bezier, -1, LINES, 1);
	SetShapeShader(data_list.size() - 1, 2);
	SetShapeMaterial(data_list.size() - 1, 2);

	for (int i = 2; i < 6; i++) {
		AddShape(Sphere, -1, TRIANGLES, 1);
		SetShapeShader(data_list.size() - 1, 2);
		SetShapeMaterial(data_list.size() - 1, 1);
		selected_data_index = data_list.size() - 1;
		ShapeTransformation(scaleAll, 0.25, 0);
	}
	AddShape(Axis, -1, LINES, 1);
	SetShapeShader(data_list.size() - 1, 2);
	SetShapeMaterial(data_list.size() - 1, 2);
	SetShapeStatic(data_list.size() - 1);

	//bezier end

	AddShape(Plane, -2, TRIANGLES, 2);
	SetShapeShader(data_list.size() - 1, 3);
	SetShapeMaterial(data_list.size() - 1, 2);
	selected_data_index = data_list.size() - 1;
	ShapeTransformation(zTranslate, -1.1, 1);
	SetShapeStatic(data_list.size() - 1);

	post_init_index = data_list.size();
	//end init

	AddShape(Cube, -1, TRIANGLES);
	SetShapeShader(data_list.size() - 1, 2);
	SetShapeMaterial(data_list.size() - 1, 1);
	previous_data_index = post_init_index;
	UpdateBezierInfo(post_init_index);
	
	selected_data_index = 0;
}

bool contains_prj(std::vector<int> v, int value) {
	if (std::count(v.begin(), v.end(), value)) {
		//std::cout << "Element found";
		return true;
	}
	return false;
}

void Project::Update(const Eigen::Matrix4f& Proj, const Eigen::Matrix4f& View, const Eigen::Matrix4f& Model, unsigned int  shaderIndx, unsigned int shapeIndx)
{
	Shader* s = shaders[shaderIndx];
	int r = ((shapeIndx) & 0x000000FF) >> 0;
	int g = ((shapeIndx) & 0x0000FF00) >> 8;
	int b = ((shapeIndx) & 0x00FF0000) >> 16;

	s->Bind();
	s->SetUniformMat4f("Proj", Proj);
	s->SetUniformMat4f("View", View);
	s->SetUniformMat4f("Model", Model);
	s->SetUniform1f("transperancy", data_list[shapeIndx]->transperancy);

	BindMaterial(s, data_list[shapeIndx]->GetMaterial());
	if (shaderIndx == 0) {
		s->SetUniform4f("lightColor", r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
	}
	else if (shaderIndx == 2 && shapeIndx != 0) 
	{
		if (shapeIndx == selected_data_index) {
			s->SetUniform4f("lightColor", 1, 0, 0, 1);
		}
		else if (contains_prj(pShapes, shapeIndx))
		{
			s->SetUniform4f("lightColor", 1, 0, 0, 1);
		}
		else {
			s->SetUniform4f("lightColor", 1, 1, 1, 1);
		}
	}
	else if (shaderIndx == 3) {
		s->SetUniform4f("lightColor", 0, 0, 1, 0.5);
	}
	s->Unbind();
}


void Project::WhenRotate()
{
}

void Project::WhenTranslate()
{
}

void Project::Animate() {
	if (isActive)
	{
		if (display_current_runtime < display_runtime) {
			MoveObjects();
			double time_passed = igl::get_seconds() - current_run_time;
			display_current_runtime = copy_current_runtime + time_passed;
		}
		else {
			Deactivate();
			current_run_time = 0;
			display_current_runtime = 0;
		}
	}
}

void Project::ScaleAllShapes(float amt, int viewportIndx)
{
	for (int i = 1; i < data_list.size(); i++)
	{
		if (data_list[i]->Is2Render(viewportIndx))
		{
			data_list[i]->MyScale(Eigen::Vector3d(amt, amt, amt));
		}
	}
}

Project::~Project(void)
{
}