#include "Project.h"
#include <iostream>


static void printMat(const Eigen::Matrix4d& mat)
{
	std::cout<<" matrix:"<<std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout<< mat(j,i)<<" ";
		std::cout<<std::endl;
	}
}

Project::Project()
{
}

//Project::Project(float angle ,float relationWH, float near, float far) : Scene(angle,relationWH,near,far)
//{ 	
//}

void Project::Init()
{		
	//unsigned int texIDs[3] = { 0 , 1, 2};
	//unsigned int slots[3] = { 0 , 1, 2 };
	//
	//AddShader("../../shaders/pickingShader");
	//AddShader("../../shaders/cubemapShader");
	//AddShader("../../shaders/basicShader");
	//AddShader("../../shaders/basicShader");
	//
	//AddTexture("../../textures/plane.png",2);
	//AddTexture("../../textures/cubemaps/Daylight Box_", 3);
	//AddTexture("../../textures/grass.bmp", 2);
	////AddTexture("../res/textures/Cat_bump.jpg", 2);

	//AddMaterial(texIDs,slots, 1);
	//AddMaterial(texIDs+1, slots+1, 1);
	//AddMaterial(texIDs + 2, slots + 2, 1);
	//
	//AddShape(Cube, -2, TRIANGLES);
	//AddShape(zCylinder, -1, TRIANGLES);
	//AddShape(zCylinder, 1, TRIANGLES);
	//AddShape(zCylinder, 2, TRIANGLES);
	//AddShape(Axis, -1, LINES);
	//AddShape(Cube, -1, TRIANGLES);

	//SetShapeShader(1, 1);
	//SetShapeShader(2, 1);
	//SetShapeShader(3, 1);
	//SetShapeShader(4, 1);
	//SetShapeShader(5, 1);


	//SetShapeMaterial(1, 0);
	//SetShapeMaterial(2, 0);	
	//SetShapeMaterial(3, 0);	
	//SetShapeMaterial(4, 0);
	//SetShapeMaterial(5, 0);


	//SetShapeMaterial(0, 1);


	//selected_data_index = 0;
	//float cylinderLen = 1.6f;
	//float s = 60;
	//ShapeTransformation(scaleAll, s,0);
	//selected_data_index = 1;
	//data()->SetCenterOfRotation(Eigen::Vector3d(0, 0, -cylinderLen / 2.0));
	//ShapeTransformation(zTranslate, cylinderLen / 2.0, 1);
	//
	//selected_data_index = 2;
	//ShapeTransformation(zTranslate, cylinderLen , 1);
	//data()->SetCenterOfRotation(Eigen::Vector3d(0, 0, -cylinderLen / 2.0));
	//
	//selected_data_index = 3;
	//ShapeTransformation(zTranslate, cylinderLen, 1);
	//data()->SetCenterOfRotation(Eigen::Vector3d(0, 0, -cylinderLen / 2.0));

	//selected_data_index = 0;
	//SetShapeStatic(0);

	unsigned int texIDs[3] = {
		 AddTexture("../../textures/cubemaps/Daylight Box_", 3),
		 AddTexture("../../textures/box0.bmp", 2),
		 AddTexture("../../textures/grass.bmp", 2)
	};

	unsigned int slots[3] = { 0 , 1 };

	AddShader("../../shaders/pickingShader");
	AddShader("../../shaders/cubemapShader");
	AddShader("../../shaders/basicShader");
	AddShader("../../shaders/pickingShader");

	for (int i = 0; i < 3; i++) {
		AddMaterial(texIDs + i, slots + i, 1);
	}


	AddShape(Cube, -2, TRIANGLES);
	AddShape(Cube, -1, TRIANGLES);
	AddShape(Cube, -1, TRIANGLES,1);
	//AddShape(Sphere, -1, TRIANGLES);
	
	

	SetShapeShader(1, 2);

	SetShapeMaterial(1, 1);
	//SetShapeShader(2, 0);

	SetShapeMaterial(0, 0);
	selected_data_index = 0;
	float s = 60;
	ShapeTransformation(scaleAll, s, 0);
	SetShapeStatic(0);
	
//	ReadPixel(); //uncomment when you are reading from the z-buffer
}

void Project::Update(const Eigen::Matrix4f& Proj, const Eigen::Matrix4f& View, const Eigen::Matrix4f& Model, unsigned int  shaderIndx, unsigned int shapeIndx)
{
	Shader *s = shaders[shaderIndx];
	int r = ((shapeIndx) & 0x000000FF) >>  0;
	int g = ((shapeIndx) & 0x0000FF00) >>  8;
	int b = ((shapeIndx) & 0x00FF0000) >> 16;


	s->Bind();
	s->SetUniformMat4f("Proj", Proj);
	s->SetUniformMat4f("View", View);
	s->SetUniformMat4f("Model", Model);
	BindMaterial(s, data_list[shapeIndx]->GetMaterial());
	s->SetUniform4f("lightColor", r / 255.0f, g / 255.0f, b / 255.0f, 0.5f);
	s->Unbind();
}


void Project::WhenRotate()
{
}

void Project::WhenTranslate()
{
}

void Project::Animate() {
    if(isActive)
	{
		if(selected_data_index > 0 )
			data()->MyRotate(Eigen::Vector3d(0, 1, 0), 0.01);
	}
}

void Project::ScaleAllShapes(float amt,int viewportIndx)
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

