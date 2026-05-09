#pragma once

#include "../../Game/game.hpp"
#include "../../Public/api.hpp"
#include "../Runtime/Renderer/renderer.hpp"
#include "../Runtime/Scene/scene.hpp"
#include "../Runtime/EntityManager/entity_manager.hpp"
#include "../engine.hpp"
#include "../../Vendor/vendor.hpp"

// https://www.reddit.com/r/cpp_questions/comments/x0ypqt/imgui_tree_view_implementation/
inline void render_tree_hierarchy(EntityManager* manager, EntityHandle entity, EntityHandle& selected) {
	Entity& entity_slot = manager->get(entity.handle);

	ImGuiTreeNodeFlags flags = (
		ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_SpanAvailWidth
	);

	bool is_leaf = entity_slot.children.count == 0;
	if (is_leaf) {
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	if (entity == selected) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	bool open = ImGui::TreeNodeEx((void*)((size_t)entity.handle.index), flags, "%s", entity_slot.name.data);
	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
		selected = entity;
	}

	if (ImGui::BeginDragDropSource()) {
		ImGui::SetDragDropPayload("ENTITY", &entity, sizeof(EntityHandle));
		ImGui::Text("  Dragging: %s", entity_slot.name.data);
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY")) {
			EntityHandle dropped = *(EntityHandle*)payload->Data;
			manager->set_parent(dropped, entity);
		}
		ImGui::EndDragDropTarget();
	}

	if (open && !is_leaf) {
		for (EntityHandle child : entity_slot.children) {
			render_tree_hierarchy(manager, child, selected);
		}
		ImGui::TreePop();
	}
}

// Theme From AI (NOTE(Jovanni): I couldn't be bothered to do this shit tbh...)
static void push_editor_style() {
	ImGuiStyle& style = ImGui::GetStyle();

	style.WindowRounding    = 6.0f;
	style.ChildRounding     = 4.0f;
	style.FrameRounding     = 4.0f;
	style.PopupRounding     = 4.0f;
	style.ScrollbarRounding = 4.0f;
	style.GrabRounding      = 4.0f;
	style.TabRounding       = 4.0f;

	style.WindowBorderSize  = 1.0f;
	style.FrameBorderSize   = 0.0f;
	style.WindowPadding     = ImVec2(10.0f, 10.0f);
	style.FramePadding      = ImVec2(6.0f, 4.0f);
	style.ItemSpacing       = ImVec2(8.0f, 6.0f);
	style.IndentSpacing     = 18.0f;
	style.ScrollbarSize     = 10.0f;

	// Color palette
	//   bg:      #0e0f11   surface: #161820   panel: #1e2028
	//   border:  #2a2d3a   accent:  #5e81f4   accent2: #7c5cbf
	//   text:    #d4d8f0   muted:   #666c8a
	ImVec4* c = ImGui::GetStyle().Colors;

	c[ImGuiCol_WindowBg]             = ImVec4(0.071f, 0.075f, 0.086f, 1.00f); // #121318
	c[ImGuiCol_ChildBg]              = ImVec4(0.082f, 0.086f, 0.098f, 1.00f);
	c[ImGuiCol_PopupBg]              = ImVec4(0.082f, 0.086f, 0.098f, 0.96f);

	c[ImGuiCol_Border]               = ImVec4(0.165f, 0.176f, 0.224f, 1.00f); // #2a2d39
	c[ImGuiCol_BorderShadow]         = ImVec4(0.00f,  0.00f,  0.00f,  0.00f);

	c[ImGuiCol_FrameBg]              = ImVec4(0.118f, 0.125f, 0.157f, 1.00f);
	c[ImGuiCol_FrameBgHovered]       = ImVec4(0.165f, 0.176f, 0.224f, 1.00f);
	c[ImGuiCol_FrameBgActive]        = ImVec4(0.200f, 0.212f, 0.270f, 1.00f);

	c[ImGuiCol_TitleBg]              = ImVec4(0.055f, 0.059f, 0.071f, 1.00f);
	c[ImGuiCol_TitleBgActive]        = ImVec4(0.071f, 0.075f, 0.098f, 1.00f);
	c[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.055f, 0.059f, 0.071f, 0.75f);

	c[ImGuiCol_MenuBarBg]            = ImVec4(0.082f, 0.086f, 0.106f, 1.00f);

	c[ImGuiCol_ScrollbarBg]          = ImVec4(0.055f, 0.059f, 0.071f, 1.00f);
	c[ImGuiCol_ScrollbarGrab]        = ImVec4(0.220f, 0.231f, 0.290f, 1.00f);
	c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.290f, 0.306f, 0.380f, 1.00f);
	c[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.369f, 0.506f, 0.957f, 1.00f); // accent

	c[ImGuiCol_CheckMark]            = ImVec4(0.369f, 0.506f, 0.957f, 1.00f);
	c[ImGuiCol_SliderGrab]           = ImVec4(0.369f, 0.506f, 0.957f, 0.90f);
	c[ImGuiCol_SliderGrabActive]     = ImVec4(0.486f, 0.608f, 1.000f, 1.00f);

	c[ImGuiCol_Button]               = ImVec4(0.165f, 0.176f, 0.224f, 1.00f);
	c[ImGuiCol_ButtonHovered]        = ImVec4(0.369f, 0.506f, 0.957f, 0.80f);
	c[ImGuiCol_ButtonActive]         = ImVec4(0.369f, 0.506f, 0.957f, 1.00f);

	c[ImGuiCol_Header]               = ImVec4(0.369f, 0.506f, 0.957f, 0.25f);
	c[ImGuiCol_HeaderHovered]        = ImVec4(0.369f, 0.506f, 0.957f, 0.40f);
	c[ImGuiCol_HeaderActive]         = ImVec4(0.369f, 0.506f, 0.957f, 0.60f);

	c[ImGuiCol_Separator]            = ImVec4(0.165f, 0.176f, 0.224f, 1.00f);
	c[ImGuiCol_SeparatorHovered]     = ImVec4(0.369f, 0.506f, 0.957f, 0.60f);
	c[ImGuiCol_SeparatorActive]      = ImVec4(0.369f, 0.506f, 0.957f, 1.00f);

	c[ImGuiCol_ResizeGrip]           = ImVec4(0.369f, 0.506f, 0.957f, 0.20f);
	c[ImGuiCol_ResizeGripHovered]    = ImVec4(0.369f, 0.506f, 0.957f, 0.60f);
	c[ImGuiCol_ResizeGripActive]     = ImVec4(0.369f, 0.506f, 0.957f, 1.00f);

	c[ImGuiCol_Tab]                  = ImVec4(0.118f, 0.125f, 0.157f, 1.00f);
	c[ImGuiCol_TabHovered]           = ImVec4(0.369f, 0.506f, 0.957f, 0.60f);
	c[ImGuiCol_TabActive]            = ImVec4(0.200f, 0.275f, 0.580f, 1.00f);
	c[ImGuiCol_TabUnfocused]         = ImVec4(0.082f, 0.086f, 0.106f, 1.00f);
	c[ImGuiCol_TabUnfocusedActive]   = ImVec4(0.140f, 0.157f, 0.210f, 1.00f);

	c[ImGuiCol_DockingPreview]       = ImVec4(0.369f, 0.506f, 0.957f, 0.50f);
	c[ImGuiCol_DockingEmptyBg]       = ImVec4(0.055f, 0.059f, 0.071f, 1.00f);

	c[ImGuiCol_PlotLines]            = ImVec4(0.369f, 0.506f, 0.957f, 1.00f);
	c[ImGuiCol_PlotLinesHovered]     = ImVec4(0.486f, 0.608f, 1.000f, 1.00f);
	c[ImGuiCol_PlotHistogram]        = ImVec4(0.369f, 0.506f, 0.957f, 1.00f);
	c[ImGuiCol_PlotHistogramHovered] = ImVec4(0.486f, 0.608f, 1.000f, 1.00f);

	c[ImGuiCol_TableHeaderBg]        = ImVec4(0.118f, 0.125f, 0.157f, 1.00f);
	c[ImGuiCol_TableBorderStrong]    = ImVec4(0.165f, 0.176f, 0.224f, 1.00f);
	c[ImGuiCol_TableBorderLight]     = ImVec4(0.118f, 0.125f, 0.157f, 1.00f);

	c[ImGuiCol_TextSelectedBg]       = ImVec4(0.369f, 0.506f, 0.957f, 0.35f);
	c[ImGuiCol_DragDropTarget]       = ImVec4(0.369f, 0.506f, 0.957f, 0.90f);
	c[ImGuiCol_NavHighlight]         = ImVec4(0.369f, 0.506f, 0.957f, 1.00f);

	c[ImGuiCol_Text]                 = ImVec4(0.831f, 0.847f, 0.941f, 1.00f); // #d4d8f0
	c[ImGuiCol_TextDisabled]         = ImVec4(0.400f, 0.424f, 0.541f, 1.00f); // #666c8a
}
// ─────────────────────────────────────────────────────────────────────────────

struct Engine;
struct Editor {
	EntityHandle selected = EntityHandle::invalid();
	ImGuizmo::OPERATION current_gizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
	// EntityHandle camera = EntityHandle::invalid();

	static Editor create(Engine* engine);

	bool render_texture_image(Engine* engine, Renderer<OpenGL>* renderer, Handle texture_handle) {
		OpenGL::Texture texture = renderer->backend.textures.get(texture_handle);
		if (texture.type == TextureSamplerType::CUBEMAP_3D) return false;

		ImGui::Image(
			(ImTextureID)(uintptr_t)texture.id,
			ImVec2(64, 64),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);

		return true;
	}

	void render_texture_image(Engine* engine, Renderer<OpenGL>* renderer, TextureHandle texture) {
		OpenGL::Texture texture_slot = renderer->backend.textures.get(texture.handle);
		ImGui::Image(
			(ImTextureID)(uintptr_t)texture_slot.id,
			ImVec2(64, 64),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);
	}

	template<typename B>
	void render(Engine* engine, Renderer<B>* renderer) {
		AppState* app = (AppState*)engine->app;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();

		push_editor_style();

		Mat4 view = engine->get_view_matrix();
		Mat4 projection = engine->get_perspective_matrix();

		{
			ImGuiWindowFlags flags = (
				ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoDocking |
				ImGuiWindowFlags_NoBringToFrontOnFocus |
				ImGuiWindowFlags_NoNavFocus
			);

			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
			ImGui::Begin("DockspaceRoot", nullptr, flags);
				ImGui::PopStyleColor();
				ImGui::PopStyleVar(2);

				ImGuiID dockspace_id = ImGui::GetID("MainDockspace");
				ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_PassthruCentralNode;

				if (!ImGui::DockBuilderGetNode(dockspace_id)) {
					ImGui::DockBuilderRemoveNode(dockspace_id);
					ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
					ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);

					ImGuiID center_id;
					ImGuiID bottom_id = ImGui::DockBuilderSplitNode(
						dockspace_id, ImGuiDir_Down, 0.25f, nullptr, &center_id
					);

					ImGuiID viewport_id;
					ImGuiID left_id = ImGui::DockBuilderSplitNode(
						center_id, ImGuiDir_Left, 0.25f, nullptr, &viewport_id
					);

					ImGuiID right_id;
					ImGui::DockBuilderSplitNode(
						viewport_id, ImGuiDir_Right, 0.5f, &right_id, nullptr
					);

					ImGui::DockBuilderDockWindow("Hierarchy", left_id);
					ImGui::DockBuilderDockWindow("Inspector", right_id);
					ImGui::DockBuilderDockWindow("Assets",    bottom_id);
					ImGui::DockBuilderFinish(dockspace_id);
				}

				ImGui::DockSpace(dockspace_id, ImVec2(0, 0), dockFlags);
			ImGui::End();
		}

		{
			ImGui::Begin("Hierarchy");
				render_tree_hierarchy(&engine->manager, engine->scene.root, this->selected);

				if (ImGui::IsWindowHovered(ImGuiHoveredFlags_None) && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered()) {
					this->selected = EntityHandle::invalid();
				}
			ImGui::End();
		}

		{
			ImGui::Begin("Inspector");
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
				if (ImGui::RadioButton("Translate", current_gizmo_operation == ImGuizmo::TRANSLATE)) {
					current_gizmo_operation = ImGuizmo::TRANSLATE;
				}
					
				ImGui::SameLine();
				if (ImGui::RadioButton("Rotate", current_gizmo_operation == ImGuizmo::ROTATE)) {
					current_gizmo_operation = ImGuizmo::ROTATE;
				}
					
				ImGui::SameLine();
				if (ImGui::RadioButton("Scale", current_gizmo_operation == ImGuizmo::SCALE)) {
					current_gizmo_operation = ImGuizmo::SCALE;
				}
					
				ImGui::PopStyleVar();
				ImGui::Separator();

				if (this->selected != EntityHandle::invalid()) {
					#define BUFFER_CAPACITY 256
					char buffer[BUFFER_CAPACITY];
					int ret = snprintf(buffer, BUFFER_CAPACITY, "Index: %d", this->selected.handle.index);
					RUNTIME_ASSERT(ret >= 0);

					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.40f, 0.42f, 0.54f, 1.00f)); // muted
					ImGui::Text("%s", buffer);
					ImGui::PopStyleColor();

					ImGui::Spacing();

					Entity& entity_slot = engine->manager.get(this->selected.handle);
					Transform& t = entity_slot.transform;

					Vec3 rotation = t.rotation.to_euler();

					ImGui::Checkbox("Active",    &entity_slot.active);
					ImGui::DragFloat3("Position", &t.position.x, 0.1f);
					ImGui::DragFloat3("Rotation", &rotation.x,   0.1f);
					ImGui::DragFloat3("Scale",    &t.scale.x,    0.1f);

					t.rotation = Quat::from_euler(rotation);

					ImGui::Spacing();

					for (const auto& entry : entity_slot.components) {
						Component* c = entry.value;
						if (MeshComponent* mesh_component = dynamic_cast<MeshComponent*>(c)) {
							if (ImGui::CollapsingHeader("MeshComponent", ImGuiTreeNodeFlags_DefaultOpen)) {
								ImGui::Indent(4.0f);
		
								ImGui::Checkbox("Render", &mesh_component->should_render);
								ImGui::Checkbox("Wireframe", &mesh_component->rasterizer_description.wireframe);
								ImGui::Checkbox("Render AABB", &mesh_component->render_aabb);
								ImGui::Spacing();

								auto mesh_slot = engine->renderer.get(mesh_component->mesh);

								ImGui::Text("Mesh");
								ImGui::Button("Drop Mesh Here", ImVec2(200, 40));
								if (ImGui::BeginDragDropTarget()) {
									if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_MESH")) {
										RUNTIME_ASSERT(payload->DataSize == sizeof(Handle));
										Handle dropped_mesh = *(Handle*)payload->Data;
										auto mesh_slot = engine->renderer.backend.meshes.get(dropped_mesh);

										mesh_component->mesh.handle = dropped_mesh;
										mesh_component->material = mesh_slot.original_material;
									}

									ImGui::EndDragDropTarget();
								}
									
								Material& material = renderer->get(mesh_component->material);
								for (auto& pair : material.bindings) {
									String key = pair.key;
									BindingValue& value = pair.value;

									ImGui::PushID(key.data);
									switch (value.type) {
										case BindingValueType::BOOL:
											ImGui::Checkbox(key.data, &value.boolean_binding);
											break;
										case BindingValueType::INTEGER:
											ImGui::DragInt(key.data, &value.integer_binding);
											break;
										case BindingValueType::FLOAT:
											ImGui::DragFloat(key.data, &value.float_binding, 0.01f);
											break;
										case BindingValueType::TEXTURE_HANDLE: {
											ImGui::Text("%s", key.data);
											ImGui::SameLine();
											if (!render_texture_image(engine, renderer, value.texture_binding.handle)) continue;
											if (ImGui::BeginDragDropTarget()) {
												if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_TEXTURE")) {
													value.texture_binding.handle = *(Handle*)payload->Data;
												}
												ImGui::EndDragDropTarget();
											}
											break;
										}
										case BindingValueType::VECTOR2:
											ImGui::DragFloat2(key.data, &value.vector2_binding.x, 0.01f);
											break;
										case BindingValueType::VECTOR3:
											ImGui::DragFloat3(key.data, &value.vector3_binding.x, 0.01f);
											break;
										case BindingValueType::VECTOR4:
											ImGui::DragFloat4(key.data, &value.vector4_binding.x, 0.01f);
											break;
										case BindingValueType::MAT4:
											ImGui::Text("%s (mat4)", key.data);
											for (int row = 0; row < 4; row++) {
												char row_label[16];
												snprintf(row_label, sizeof(row_label), "##row%d", row);
												ImGui::DragFloat4(row_label, &value.mat4_binding.v[row].x, 0.01f);
											}
											break;
									}
									ImGui::PopID();
								}
						

								ImGui::Spacing();
								ImGui::Separator();
								ImGui::Text("Add Binding");

								/*
								static char new_key[64] = {};
								static int new_type = 0;
								const char* type_names[] = { "Bool", "Integer", "Float", "Texture", "Vec2", "Vec3", "Vec4", "Mat4" };
								ImGui::InputText("Key##new", new_key, sizeof(new_key));
								ImGui::Combo("Type##new", &new_type, type_names, IM_ARRAYSIZE(type_names));
								if (ImGui::Button("Add##binding") && new_key[0] != '\0') {
									BindingValue new_val = {};
									new_val.type = (BindingValueType)new_type;
									material.bindings.put(String::create(new_key, ), new_val);
									new_key[0] = '\0';
								}

								ImGui::SameLine();
								if (ImGui::Button("Remove Selected##binding")) {
									ImGui::OpenPopup("RemoveBindingPopup");
								}

								if (ImGui::BeginPopup("RemoveBindingPopup")) {
									for (auto& pair : material.bindings) {
										if (ImGui::MenuItem(pair.key.data)) {
											material.bindings.remove(pair.key);
											ImGui::CloseCurrentPopup();
											break;
										}
									}
									ImGui::EndPopup();
								}
								*/

								ImGui::Unindent(4.0f);
							}
						} else if (SkyboxComponent* skybox_component = dynamic_cast<SkyboxComponent*>(c)) {
							if (ImGui::CollapsingHeader("SkyboxComponent", ImGuiTreeNodeFlags_DefaultOpen)) {
								ImGui::Indent(4.0f);

								ImGui::Unindent(4.0f);
							}
						} else if (CameraComponent* camera_component = dynamic_cast<CameraComponent*>(c)) {
							if (ImGui::CollapsingHeader("CameraComponent", ImGuiTreeNodeFlags_DefaultOpen)) {
								ImGui::Indent(4.0f);

								ImGui::Unindent(4.0f);
							}
						} else if (FirstPersonCameraControllerComponent* camera_controller = dynamic_cast<FirstPersonCameraControllerComponent*>(c)) {
							if (ImGui::CollapsingHeader("FirstPersonCameraControllerComponent", ImGuiTreeNodeFlags_DefaultOpen)) {
								ImGui::Indent(4.0f);

								ImGui::Unindent(4.0f);
							}
						} else {
							RUNTIME_ASSERT(false);
						}
					}

					if (ImGui::Button("Add Component")) {
						ImGui::OpenPopup("AddComponentPopup");
					}

					if (ImGui::BeginPopup("AddComponentPopup")) {

						bool has_mesh = entity_slot.has_component<MeshComponent>();
						bool has_skybox = entity_slot.has_component<SkyboxComponent>();

						if (!has_mesh) {
							if (ImGui::MenuItem("MeshComponent")) {
								// entity_slot.add_component<MeshComponent>();
								ImGui::CloseCurrentPopup();
							}
						}

						if (!has_skybox) {
							if (ImGui::MenuItem("SkyboxComponent")) {
								// entity_slot.add_component<SkyboxComponent>();
								ImGui::CloseCurrentPopup();
							}
						}

						ImGui::EndPopup();
					}

					// 2. Define the modal window
					if (ImGui::BeginPopupModal("Add Component Modal", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
						ImGui::Text("This is a modal window.");
						ImGui::Separator();

						if (ImGui::Button("Close", ImVec2(120, 0))) { 
							ImGui::CloseCurrentPopup(); 
						}
						ImGui::EndPopup();
					}
				} else {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.40f, 0.42f, 0.54f, 1.00f));
					ImGui::TextWrapped("No entity selected.");
					ImGui::PopStyleColor();
				}
			ImGui::End();
		}

		{
			ImGuiWindowFlags flags = (
				ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoScrollbar |
				ImGuiWindowFlags_NoScrollWithMouse |
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoSavedSettings |
				ImGuiWindowFlags_NoMouseInputs |
				ImGuiWindowFlags_NoBackground
			);

			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImVec2((float)engine->window.WINDOW_WIDTH, (float)engine->window.WINDOW_HEIGHT));
			ImGui::Begin("GizmoOverlay", nullptr, flags);
				if (this->selected != EntityHandle::invalid()) {
					Entity& entity_slot = engine->manager.get(this->selected.handle);

					ImGuizmo::SetOrthographic(false);
					ImGuizmo::SetDrawlist();
					ImGuizmo::SetRect(0, 0, (float)engine->window.WINDOW_WIDTH, (float)engine->window.WINDOW_HEIGHT);

					Mat4 viewT         = view.transpose();
					Mat4 projT         = projection.transpose();
					Mat4 newTransformT = engine->manager.get_world_transform(this->selected).transpose();

					bool is_manipulated = ImGuizmo::Manipulate(
						&viewT.v[0].x, &projT.v[0].x,
						current_gizmo_operation, ImGuizmo::WORLD,
						&newTransformT.v[0].x
					);

					if (is_manipulated) {
						Mat4 newTransformWorld = newTransformT.transpose();
						engine->manager.set_world_transform(this->selected, newTransformWorld);
					}
				}
			ImGui::End();
		}

		{
			ImGui::Begin("Assets");
				Vector<Handle> texture_handles = renderer->backend.textures.handle_list(engine->memory.frame_allocator);
				for (int i = 0; i < texture_handles.count; i++) {
					Handle texture_handle = texture_handles[i];

					if (!render_texture_image(engine, renderer, texture_handle)) continue;
					ImGui::SameLine();

					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
						ImGui::SetDragDropPayload("ASSET_TEXTURE", &texture_handle, sizeof(Handle));
						ImGui::Text("Dragging: texture %d", i);
						ImGui::EndDragDropSource();
					}
				}

				ImGui::Separator();

				Vector<Handle> mesh_handles = renderer->backend.meshes.handle_list(engine->memory.frame_allocator);
				for (int i = 0; i < mesh_handles.count; i++) {
					Handle mesh = mesh_handles[i];

					ImGui::Text("Mesh %d", i);
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
						ImGui::SetDragDropPayload("ASSET_MESH", &mesh, sizeof(Handle));
						ImGui::Text("Mesh %d", i);   // safe: i is still in scope
						ImGui::EndDragDropSource();
					}

					ImGui::SameLine();
				}

				ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
};