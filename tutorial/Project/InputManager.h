#pragma once   //maybe should be static class
#include "igl/opengl/glfw/Display.h"
#include "igl/opengl/glfw/renderer.h"
#include "Project.h"
#include "imgui/imgui.h"


	int findViewPort(GLFWwindow* window, double xpos, double ypos) {
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);

		for (int i = 0; i < 2; i++) {
			if (rndr->CheckViewport(xpos, ypos, i)) {
				return i;
			}
		}
		return 0;
	}

	void glfw_mouse_callback(GLFWwindow* window,int button, int action, int mods)
	{	
		if (action == GLFW_PRESS)
		{
			Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
			Project* scn = (Project*)rndr->GetScene();
			double x2, y2;
			
			glfwGetCursorPos(window, &x2, &y2);
			
			
			
			if (x2 > 400)
			{
				rndr->UpdatePress(x2, y2);
				if (button == GLFW_MOUSE_BUTTON_RIGHT && scn->region_selection)
				{
					rndr->RightPressed();
				}
				else
				{
					if (rndr->Picking_2((int)x2, (int)y2, findViewPort(window, x2, y2)))
					{
						rndr->UpdatePosition(x2, y2);
						if (button == GLFW_MOUSE_BUTTON_LEFT)
							rndr->Pressed();
					}
					else
					{
						rndr->UnPick(2);
					}
				}
			}
		
		}
		else
		{
			Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
			Project* scn = (Project*)rndr->GetScene();
			rndr->UnPick(2);

			if (rndr->IsRightPressed())
			{
				rndr->RightPressed();
				if (rndr->isZoom)
				{
					
					rndr->MakeZoom();
					
					scn->region_selection = false;
				}
				else if (scn->region_selection) {
					rndr->PickMany(0);
					scn->region_selection = false;
				}
			}
			
		}
	}
	
	void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
		Project* scn = (Project*)rndr->GetScene();
		
		if (rndr->IsPicked())
		{
			rndr->UpdateZpos((int)yoffset);
			rndr->MouseProccessing(GLFW_MOUSE_BUTTON_MIDDLE);
		}
		else
		{
			int chosen_view_port_camera = findViewPort(window, xoffset, yoffset); //todo WIP Currently doesnt seem to switch view port
			rndr->MoveCamera(chosen_view_port_camera, rndr->zTranslate, (float)yoffset);
		}
		
	}



	
	void glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
		Project* scn = (Project*)rndr->GetScene();

		rndr->UpdatePosition((float)xpos,(float)ypos);
		int view_port_index = findViewPort(window, xpos, ypos);

		double x2, y2;

		glfwGetCursorPos(window, &x2, &y2);

		//if (rndr->CheckViewport(xpos,ypos, 0))
		//{
		if (x2 > 400)
		{
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
			{

				rndr->MouseProccessing(GLFW_MOUSE_BUTTON_RIGHT, 0, view_port_index);
			}
			else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			{

				rndr->MouseProccessing(GLFW_MOUSE_BUTTON_LEFT, 0, view_port_index);
			}
			else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE && rndr->IsPicked() && rndr->IsMany())
				rndr->MouseProccessing(GLFW_MOUSE_BUTTON_RIGHT, 0, view_port_index);
		}

		//}
	}

	void glfw_window_size_callback(GLFWwindow* window, int width, int height)
	{
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);

        rndr->resize(window,width,height);
		
	}
	
	void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		int objIdx = 0;
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
		Project* scn = (Project*)rndr->GetScene();
		//rndr->FreeShapes(2);
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			switch (key)
			{
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, GLFW_TRUE);
				break;
				
			case GLFW_KEY_SPACE:
				scn->SetAnimation();
				break;

			case GLFW_KEY_UP:
				rndr->MoveCamera(rndr->CurrentCameraIdx, scn->xRotate, 0.05f);
				break;
			case GLFW_KEY_DOWN:
				rndr->MoveCamera(rndr->CurrentCameraIdx, scn->xRotate, -0.05f);
				break;
			case GLFW_KEY_LEFT:
				rndr->MoveCamera(rndr->CurrentCameraIdx, scn->yRotate, 0.05f);
				break;
			case GLFW_KEY_RIGHT:
				rndr->MoveCamera(rndr->CurrentCameraIdx, scn->yRotate, -0.05f);
				break;
			case GLFW_KEY_U:
				rndr->MoveCamera(rndr->CurrentCameraIdx, scn->yTranslate, 0.25f);
				break;
			case GLFW_KEY_D:
				rndr->MoveCamera(rndr->CurrentCameraIdx, scn->yTranslate, -0.25f);
				break;
			case GLFW_KEY_L:
				rndr->MoveCamera(rndr->CurrentCameraIdx, scn->xTranslate, -0.25f);
				break;

			
			case GLFW_KEY_R:
				rndr->MoveCamera(rndr->CurrentCameraIdx, scn->xTranslate, 0.25f);
				break;
			
			case GLFW_KEY_B:
				rndr->MoveCamera(rndr->CurrentCameraIdx, scn->zTranslate, 0.5f);
				break;
			case GLFW_KEY_F:
				rndr->MoveCamera(rndr->CurrentCameraIdx, scn->zTranslate, -0.5f);
				break;

			case GLFW_KEY_BACKSPACE:
				scn->pShapes.clear();
				rndr->isMany = false;
				break;

			case GLFW_KEY_1:
				if (scn->selected_data_index >= scn->post_init_index)
				{
					scn->ShapeTransformation(scn->zTranslate, -1, 0);
				}
				break;
			case GLFW_KEY_2:
				if (scn->selected_data_index >= scn->post_init_index)
				{
					scn->ShapeTransformation(scn->zTranslate, 1, 0);
				}
				break;
			case GLFW_KEY_3:
			{
				Eigen::Matrix4f prj = rndr->cameras[0]->_projection;
				rndr->isZoom = !rndr->isZoom;
				std::cout << "isZoom: " << rndr->isZoom << std::endl;
				break;
			}

			case GLFW_KEY_4:
				rndr->cameras[0]->SetTout(rndr->origCameraLocation);
				break;

			case GLFW_KEY_S:
				rndr->SwitchCameraIdx();

				break;
			default:
				break;

			}
		}
	}


void Init(Display& display, igl::opengl::glfw::imgui::ImGuiMenu *menu)
{
    display.AddKeyCallBack(glfw_key_callback);
    display.AddMouseCallBacks(glfw_mouse_callback, glfw_scroll_callback, glfw_cursor_position_callback);
    display.AddResizeCallBack(glfw_window_size_callback);
    menu->init(&display);
}
