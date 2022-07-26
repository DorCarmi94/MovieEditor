#include "igl/opengl/glfw/renderer.h"
#include "Project.h"
#include "InputManager.h"

int main(int argc,char *argv[])
{
	const int DISPLAY_WIDTH = 1600;
	const int DISPLAY_HEIGHT = 800;
	const float CAMERA_ANGLE = 45.0f;
	const float NEAR = 1.0f;
	const float FAR = 300.0f;
	const int infoIndx = 2; 
	std::list<int> x, y;
	x.push_back(400+(DISPLAY_WIDTH-400)/2);
	x.push_back(DISPLAY_WIDTH);
	y.push_back(DISPLAY_HEIGHT);
    Display disp = Display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "OPENGL");
    igl::opengl::glfw::imgui::ImGuiMenu* menu = new igl::opengl::glfw::imgui::ImGuiMenu();
    Renderer* rndr = new Renderer(CAMERA_ANGLE, ((float)DISPLAY_WIDTH-400)/(float)DISPLAY_HEIGHT/2, NEAR, FAR);
	menu->rndr = rndr;
	Project *scn = new Project();  //initializing scene
	
    Init(disp,menu); //adding callback functions
	
	scn->Init();    //adding shaders, textures, shapes to scene
    rndr->Init(scn,x,y,1,menu); // adding scene and viewports to the renderer

	scn->rndr = rndr;

	rndr->CopyDraw(0, rndr->viewport, 1);

	//rndr->AddCamera(Eigen::Vector3d(0, 0, 10), CAMERA_ANGLE, ((float)DISPLAY_WIDTH - 400) / (float)DISPLAY_HEIGHT / 2, NEAR, FAR, 2);
	//Scissor Square
	rndr->AddViewport(400, 0, 600, 800);
	rndr->CopyDraw(1, rndr->viewport, 2);
	int size = rndr->drawInfos.size();
	rndr->ClearDrawFlag(size - 1, rndr->toClear|rndr->stencilTest);
	rndr->SetDrawFlag(size - 1, rndr->blend|rndr->inAction2|rndr->scissorTest);
	

	disp.SetRenderer(rndr);

    disp.launch_rendering(rndr);

	delete scn;
	delete rndr;
	delete menu;
	
	return 0;
}

