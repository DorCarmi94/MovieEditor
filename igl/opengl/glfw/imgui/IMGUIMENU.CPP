// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2018 Jérémie Dumas <jeremie.dumas@ens-lyon.org>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
////////////////////////////////////////////////////////////////////////////////
//#include "ImGuiMenu.h"
//#include "ImGuiHelpers.h"
#include <igl/project.h>
#include "ImGuiHelpers.h"

#include "ImGuiMenu.h"
#include "../imgui.h"
#include "igl/opengl/glfw/imgui/imgui_impl_glfw.h"
#include "igl/opengl/glfw/imgui/imgui_impl_opengl3.h"

//#include <imgui_fonts_droid_sans.h>
//#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <igl/file_dialog_open.h>
////////////////////////////////////////////////////////////////////////////////

namespace igl
{
    namespace opengl
    {
        namespace glfw
        {
            namespace imgui
            {

                IGL_INLINE void ImGuiMenu::init(Display* disp)
                {
                    // Setup ImGui binding
                    if (disp->window)
                    {
                        IMGUI_CHECKVERSION();
                        if (!context_)
                        {
                            // Single global context by default, but can be overridden by the user
                            static ImGuiContext* __global_context = ImGui::CreateContext();
                            context_ = __global_context;
                        }
                        const char* glsl_version = "#version 150";

                        ImGui_ImplGlfw_InitForOpenGL(disp->window, true);
                        ImGui_ImplOpenGL3_Init(glsl_version);
                        ImGui::GetIO().IniFilename = nullptr;
                        ImGui::StyleColorsDark();
                        ImGuiStyle& style = ImGui::GetStyle();
                        style.FrameRounding = 5.0f;
                        reload_font();
                    }
                }

                IGL_INLINE void ImGuiMenu::reload_font(int font_size)
                {
                    hidpi_scaling_ = hidpi_scaling();
                    pixel_ratio_ = pixel_ratio();
                    ImGuiIO& io = ImGui::GetIO();
                    io.Fonts->Clear();
                    // io.Fonts->AddFontFromMemoryCompressedTTF(droid_sans_compressed_data,
                    //   droid_sans_compressed_size, font_size * hidpi_scaling_);
                    io.FontGlobalScale = 1.0 / pixel_ratio_;
                }

                IGL_INLINE void ImGuiMenu::shutdown()
                {
                    // Cleanup
                    ImGui_ImplOpenGL3_Shutdown();
                    ImGui_ImplGlfw_Shutdown();
                    // User is responsible for destroying context if a custom context is given
                    // ImGui::DestroyContext(*context_);
                }

                IGL_INLINE bool ImGuiMenu::pre_draw()
                {
                    glfwPollEvents();

                    // Check whether window dpi has changed
                    float scaling = hidpi_scaling();
                    if (std::abs(scaling - hidpi_scaling_) > 1e-5)
                    {
                        reload_font();
                        ImGui_ImplOpenGL3_DestroyDeviceObjects();
                    }

                    ImGui_ImplOpenGL3_NewFrame();
                    ImGui_ImplGlfw_NewFrame();
                    ImGui::NewFrame();
                    return false;
                }

                IGL_INLINE bool ImGuiMenu::post_draw()
                {
                    //draw_menu(viewer,core);
                    ImGui::Render();
                    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                    return false;
                }

                IGL_INLINE void ImGuiMenu::post_resize(int width, int height)
                {
                    if (context_)
                    {
                        ImGui::GetIO().DisplaySize.x = float(width);
                        ImGui::GetIO().DisplaySize.y = float(height);
                    }
                }

                // Mouse IO
                IGL_INLINE bool ImGuiMenu::mouse_down(GLFWwindow* window, int button, int modifier)
                {
                    ImGui_ImplGlfw_MouseButtonCallback(window, button, GLFW_PRESS, modifier);
                    return ImGui::GetIO().WantCaptureMouse;
                }

                IGL_INLINE bool ImGuiMenu::mouse_up(GLFWwindow* window, int button, int modifier)
                {
                    //return ImGui::GetIO().WantCaptureMouse;
                    // !! Should not steal mouse up
                    return false;
                }

                IGL_INLINE bool ImGuiMenu::mouse_move(GLFWwindow* window, int mouse_x, int mouse_y)
                {
                    return ImGui::GetIO().WantCaptureMouse;
                }

                IGL_INLINE bool ImGuiMenu::mouse_scroll(GLFWwindow* window, float delta_y)
                {
                    ImGui_ImplGlfw_ScrollCallback(window, 0.f, delta_y);
                    return ImGui::GetIO().WantCaptureMouse;
                }

                // Keyboard IO
                IGL_INLINE bool ImGuiMenu::key_pressed(GLFWwindow* window, unsigned int key, int modifiers)
                {
                    ImGui_ImplGlfw_CharCallback(nullptr, key);
                    return ImGui::GetIO().WantCaptureKeyboard;
                }

                IGL_INLINE bool ImGuiMenu::key_down(GLFWwindow* window, int key, int modifiers)
                {
                    ImGui_ImplGlfw_KeyCallback(window, key, 0, GLFW_PRESS, modifiers);
                    return ImGui::GetIO().WantCaptureKeyboard;
                }

                IGL_INLINE bool ImGuiMenu::key_up(GLFWwindow* window, int key, int modifiers)
                {
                    ImGui_ImplGlfw_KeyCallback(window, key, 0, GLFW_RELEASE, modifiers);
                    return ImGui::GetIO().WantCaptureKeyboard;
                }

                bool ButtonCenteredOnLine(const char* label, float alignment = 0.5f)
                {
                    ImGuiStyle& style = ImGui::GetStyle();

                    float size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
                    float avail = ImGui::GetContentRegionAvail().x;

                    float off = (avail - size) * alignment;
                    if (off > 0.0f)
                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

                    return ImGui::Button(label);
                }

                IGL_INLINE void ImGuiMenu::draw_viewer_menu(igl::opengl::glfw::Viewer* viewer, std::vector<igl::opengl::Camera*>& camera, Eigen::Vector4i& viewWindow, std::vector<DrawInfo*> drawInfos)
                {
                    bool* p_open = NULL;
                    static bool no_titlebar = false;
                    static bool no_scrollbar = false;
                    static bool no_menu = true;
                    static bool no_move = false;
                    static bool no_resize = false;
                    static bool no_collapse = false;
                    static bool no_close = false;
                    static bool no_nav = false;
                    static bool no_background = false;
                    static bool no_bring_to_front = false;

                    ImGuiWindowFlags window_flags = 0;
                    if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
                    if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
                    if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
                    if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
                    if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
                    if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
                    if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
                    if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
                    if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
                    ImGui::Begin(
                        "Viewer", p_open,
                        window_flags
                    );

                    ImGui::SetWindowPos(ImVec2((float)0, (float)0), ImGuiCond_Always);
                    ImGui::SetWindowSize(ImVec2((float)0, (float)0), ImGuiCond_Always);
                    ImGui::End();
                    no_move = true;
                    no_resize = true;
                    ImGui::Begin(
                        "Viewer", p_open,
                        window_flags
                    );

                    // Mesh
                    if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        float w = ImGui::GetContentRegionAvailWidth();
                        float p = ImGui::GetStyle().FramePadding.x;
                        if (ImGui::Button("Load##Mesh", ImVec2((w - p) / 2.f, 0)))
                        {
                            int savedIndx = viewer->selected_data_index;
                            // viewer->selected_data_index = viewer->parents.size();
                            // viewer->AddShape(viewer->xCylinder,-1,viewer->TRIANGLES);
                            viewer->open_dialog_load_mesh();
                            if (viewer->data_list.size() > viewer->parents.size())
                            {

                                viewer->parents.push_back(-1);
                                viewer->SetShapeViewport(viewer->selected_data_index, 0);
                                viewer->SetShapeShader(viewer->selected_data_index, 2);
                                viewer->SetShapeMaterial(viewer->selected_data_index, 0);
                                //viewer->data_list.back()->set_visible(false, 1);
                                //viewer->data_list.back()->set_visible(true, 2);
                                viewer->data_list.back()->UnHide();
                                viewer->data_list.back()->show_faces = 3;
                                viewer->data()->mode = viewer->TRIANGLES;
                                viewer->selected_data_index = savedIndx;
                            }
                        }
                        ImGui::SameLine(0, p);
                        if (ImGui::Button("Save##Mesh", ImVec2((w - p) / 2.f, 0)))
                        {
                            viewer->open_dialog_save_mesh();
                        }
                        if (viewer->selected_data_index > 6) {
                            ImGui::SliderFloat("Opacity", &viewer->data_list[viewer->selected_data_index]->transperancy, 0, 1);
                        }

                    }

                    // Material
                    if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        //todo seems like selected_data_index reverts back to 0, needs to be fixed
                        float w = ImGui::GetContentRegionAvailWidth();
                        float p = ImGui::GetStyle().FramePadding.x;
                        if (ImGui::Button("change##Material", ImVec2(-1, 0)))
                        {
                            int saved_texture_Indx = viewer->texture_index;
                            viewer->open_dialog_load_material();
                            if (viewer->texture_index > saved_texture_Indx)
                            {
                                int mtr_idx = viewer->AddMaterial(viewer->texIDs + viewer->material_index, viewer->slots + viewer->material_index, 1);
                                viewer->SetShapeMaterial(viewer->selected_data_index, mtr_idx);
                            }
                        }

                    }

                    //Background
                    if (ImGui::CollapsingHeader("Background", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        if (ImGui::Button("Switch Background##Background")) {
                            ImGui::OpenPopup("popup window");
                        }

                        bool open = true;
                        if (ImGui::BeginPopupModal("popup window", &open))
                        {
                            ImGui::Text("Choose background walls in the following order:\nRight, Left, Top, Bottom, Front, Back");
                            float w = ImGui::GetContentRegionAvailWidth();
                            float p = ImGui::GetStyle().FramePadding.x;
                            if (ButtonCenteredOnLine("Close")) {
                                ImGui::CloseCurrentPopup();
                                open = false;
                                std::vector<std::string> fileNames;
                                bool verified = true;

                                for (int i = 0; i < 6; i++) {
                                    fileNames.push_back(igl::file_dialog_open());
                                    if (fileNames[i].length() == 0) {
                                        verified = false;
                                        break;
                                    }
                                }
                                if (verified) {
                                    viewer->AddTextureForCubeMap(fileNames);
                                    int mtr_idx = viewer->AddMaterial(viewer->texIDs + viewer->material_index, viewer->slots + viewer->material_index, 1);
                                    viewer->SetShapeMaterial(0, mtr_idx);
                                }
                            }
                            ImGui::EndPopup();
                        }
                    }

                    static int chosen_layer = 0;
                    //Layers
                    if (ImGui::CollapsingHeader("Layers", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        if (ImGui::Button("Add Layer##Layers", ImVec2(-1, 0)))
                        {
                            viewer->AddLayer();
                        }

                        if (ImGui::Button("Hide Layer##Layers", ImVec2(-1, 0))) //todo implement better
                        {
                            for (int i = 0; i < viewer->data_list.size(); i++) {
                                if (viewer->data_list[i]->layer == chosen_layer) {
                                    viewer->data_list[i]->Hide();
                                }
                            }
                        }
                        if (ImGui::Button("Unhide Layer##Layers", ImVec2(-1, 0)))
                        {
                            for (int i = 0; i < viewer->data_list.size(); i++) {
                                if (viewer->data_list[i]->layer == chosen_layer) {
                                    viewer->data_list[i]->UnHide();
                                }
                            }
                        }

                        if (ImGui::Button("Switch Layer##Layers", ImVec2(-1, 0)))
                        {
                            viewer->ChangeLayer(chosen_layer);
                        }

                        ImGui::SliderInt("Choose layer", &chosen_layer, 0, viewer->layers);
                    }

                    //Cameras
                    if (ImGui::CollapsingHeader("Cameras", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        float w = ImGui::GetContentRegionAvailWidth();
                        float p = ImGui::GetStyle().FramePadding.x;
                        if (ImGui::Button("Add Camera##Cameras", ImVec2((w - p) / 2.f, 0)))
                        {
                            //to impl
                        }
                        ImGui::SameLine(0, p);
                        if (ImGui::Button("Switch Camera##Cameras", ImVec2((w - p) / 2.f, 0)))
                        {
                            //to impl
                        }
                    }

                    // Animation Time
                    if (ImGui::CollapsingHeader("Animate", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        float w = ImGui::GetContentRegionAvailWidth();
                        float p = ImGui::GetStyle().FramePadding.x;

                        if (ImGui::Button("Play##Animate", ImVec2((w - p) / 2.f, 0)))
                        {
                            viewer->SetAnimation();
                        }
                        ImGui::PushItemWidth(80 * menu_scaling());
                        ImGui::DragFloat("Animation Run Time", &(viewer->display_runtime), 0.05f, 1.0f, 90.0f);
                        ImGui::PopItemWidth();
                        ImGui::SliderFloat("Current Run Time", &viewer->display_current_runtime, 0, viewer->display_runtime);

                    }

                    // Viewing options
                    if (ImGui::CollapsingHeader("Viewing Options", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        if (ImGui::Button("Center object", ImVec2(-1, 0)))
                        {
                            std::cout << "not implemented yet" << std::endl;
                            //      core[1].align_camera_center(viewer->data().V, viewer->data().F); TODO: add function like this to camera
                        }
                        //if (ImGui::Button("Snap canonical view", ImVec2(-1, 0)))
                        //{
                        //  core[1].snap_to_canonical_quaternion();
                        //}

                        // Zoom
                        ImGui::PushItemWidth(80 * menu_scaling());
                        if (camera[0]->_ortho)
                            ImGui::DragFloat("Zoom", &(camera[0]->_length), 0.05f, 0.1f, 20.0f);
                        else
                            ImGui::DragFloat("Fov", &(camera[0]->_fov), 0.05f, 30.0f, 90.0f);

                        // Select rotation type
                        static Eigen::Quaternionf trackball_angle = Eigen::Quaternionf::Identity();
                        static bool orthographic = true;

                        // Orthographic view
                        ImGui::Checkbox("Orthographic view", &(camera[0]->_ortho));
                        if (camera[0]->_ortho) {
                            camera[0]->SetProjection(0, camera[0]->_relationWH);
                        }
                        else {
                            camera[0]->SetProjection(camera[0]->_fov > 0 ? camera[0]->_fov : 45, camera[0]->_relationWH);
                        }

                        ImGui::PopItemWidth();
                    }

                    // Helper for setting viewport specific mesh options
                    auto make_checkbox = [&](const char* label, unsigned int& option)
                    {
                        return ImGui::Checkbox(label,
                            [&]() { return drawInfos[1]->is_set(option); },
                            [&](bool value) { return drawInfos[1]->set(option, value); }
                        );
                    };
                    ImGui::ColorEdit4("Background", drawInfos[1]->Clear_RGBA.data(),
                        ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);

                    // Draw options
                    if (ImGui::CollapsingHeader("Draw Options", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        if (ImGui::Checkbox("Face-based", &(viewer->data()->face_based)))
                        {
                            viewer->data()->dirty = MeshGL::DIRTY_ALL;
                        }
                        //
                        //    make_checkbox("Show texture", viewer->data().show_texture);
                        //    if (ImGui::Checkbox("Invert normals", &(viewer->data().invert_normals)))
                        //    {
                        //      viewer->data().dirty |= igl::opengl::MeshGL::DIRTY_NORMAL;
                        //    }
                        make_checkbox("Show overlay", viewer->data()->show_overlay);
                        make_checkbox("Show overlay depth", viewer->data()->show_overlay_depth);

                        ImGui::ColorEdit4("Line color", viewer->data()->line_color.data(),
                            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
                        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.3f);
                        ImGui::DragFloat("Shininess", &(viewer->data()->shininess), 0.05f, 0.0f, 100.0f);
                        ImGui::PopItemWidth();
                    }

                    // Overlays
                    if (ImGui::CollapsingHeader("Overlays", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        make_checkbox("Wireframe", viewer->data()->show_lines);
                        make_checkbox("Fill", viewer->data()->show_faces);

                    }
                    ImGui::End();
                }



                IGL_INLINE float ImGuiMenu::pixel_ratio()
                {
                    // Computes pixel ratio for hidpi devices
                    int buf_size[2];
                    int win_size[2];
                    GLFWwindow* window = glfwGetCurrentContext();
                    glfwGetFramebufferSize(window, &buf_size[0], &buf_size[1]);
                    glfwGetWindowSize(window, &win_size[0], &win_size[1]);
                    return (float)buf_size[0] / (float)win_size[0];
                }

                IGL_INLINE float ImGuiMenu::hidpi_scaling()
                {
                    // Computes scaling factor for hidpi devices
                    float xscale, yscale;
                    GLFWwindow* window = glfwGetCurrentContext();
                    glfwGetWindowContentScale(window, &xscale, &yscale);
                    return 0.5 * (xscale + yscale);
                }

            } // end namespace
        } // end namespace
    } // end namespace
} // end namespace
