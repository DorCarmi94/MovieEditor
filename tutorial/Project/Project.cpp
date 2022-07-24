#include "Project.h"
#include <iostream>


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

//Project::Project(float angle ,float relationWH, float near, float far) : Scene(angle,relationWH,near,far)
//{ 	
//}

void Project::Init()
{
	unsigned int texIDs[3] = {
		 AddTexture("../../textures/cubemaps/Daylight Box_", 3),
		 AddTexture("../../textures/box0.bmp", 2),
	     AddTexture("../../textures/plane.png", 2)
	};

	unsigned int slots[3] = { 0 , 1 };

	AddShader("../../shaders/pickingShader");
	AddShader("../../shaders/cubemapShader");
	AddShader("../../shaders/basicShader");
	AddShader("../../shaders/pickingShader");

	for (int i = 0; i < 3; i++) {
		AddMaterial(texIDs + i, slots + i, 1);
	}

	//skybox start
	AddShape(Cube, -2, TRIANGLES);	
	SetShapeShader(shapesCounter, 1);
	SetShapeMaterial(shapesCounter, 0);
	selected_data_index = 0;
	float s = 60;
	ShapeTransformation(scaleAll, s, 0);
	SetShapeStatic(0);
	//skybox end

	//bezier start
	shapesCounter++;
	AddShape(Bezier, -1, LINES);
	SetShapeShader(shapesCounter, 2);
	SetShapeMaterial(shapesCounter, 2);

	selected_data_index = 1;
	//ShapeTransformation(scaleAll, 0.1, 0);

	for (int i = 2; i < 6; i++) {
		shapesCounter++;
		AddShape(Sphere, -1, TRIANGLES);
		SetShapeShader(shapesCounter, 2);
		SetShapeMaterial(shapesCounter, 1);
		selected_data_index = shapesCounter;
		ShapeTransformation(scaleAll, 0.25, 0);
	}
	shapesCounter++;
	AddShape(Axis, -1, LINES, 1);
	SetShapeShader(shapesCounter, 2);
	SetShapeMaterial(shapesCounter, 2);
	SetShapeStatic(shapesCounter);

	//bezier end
	shapesCounter++;
	AddShape(Cube, -1, TRIANGLES);
	SetShapeShader(shapesCounter, 2);
	SetShapeMaterial(shapesCounter, 1);

	shapesCounter++;
	AddShape(Cube, -1, TRIANGLES);
	SetShapeShader(shapesCounter, 2);
	SetShapeMaterial(shapesCounter, 1);

	

	shapesCounter++;
	AddShape(Cube, -1, TRIANGLES,2);
	SetShapeShader(shapesCounter, 2);
	SetShapeMaterial(shapesCounter, 1);


	shapesCounter++;
	AddShape(Cube, -1, TRIANGLES,1);
	SetShapeShader(shapesCounter, 2);
	SetShapeMaterial(shapesCounter, 1);



	std::vector<Eigen::Vector3d> p_bezier;
	p_bezier.push_back(Eigen::Vector3d(18.8, -26.6, 0));
	p_bezier.push_back(Eigen::Vector3d(2.6, 12.9, 0));
	p_bezier.push_back(Eigen::Vector3d(-2, -15.6, 0));
	p_bezier.push_back(Eigen::Vector3d(29.5, 33, 0));
	data_list[7]->UpdateBezierCP(p_bezier);
	//UpdateBezierInfo();
	

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
	else if (shaderIndx == 2 && shapeIndx != 0) {
		s->SetUniform4f("lightColor", r / 255.0f, g / 255.0f, b / 255.0f, data_list[shapeIndx]->transperancy);
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
		//if (selected_data_index > 0)
			//MoveObjects();
			//data()->MyRotate(Eigen::Vector3d(0, 1, 0), 0.01);
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