# Two Main Drawing Modes in ImGui
## 1. UI Widgets (Standard ImGui API)

This is the bread and butter — ImGui’s core API gives you ready-made widgets, e.g.:
| Widget Type	| Function |
| ------------- | ---------|
| Text          | `ImGui::Text("Hello %d", 42)`       |
| Button	    | `ImGui::Button("Click Me")`         |
| Checkbox	    | `ImGui::Checkbox("Enabled", &b)`    |
| Slider	    | `ImGui::SliderFloat("X", &x, 0, 1)` |
| Combo / Dropdown | `ImGui::Combo(...)`              |
| Input Field	| `ImGui::InputText(...)`             |
| Tree Node	    | `ImGui::TreeNode("Header")`         |
| Collapsing Header	| `ImGui::CollapsingHeader(...)`  |
| Color Picker	| `ImGui::ColorEdit3(...)`            |
| Progress Bar	| `ImGui::ProgressBar(0.5f)`          |
| Image	        | `ImGui::Image(texture_id, size)`    |

These are automatic layout widgets, drawn inside `ImGui::Begin()/End()` windows.
## 2. Custom Draw Commands (ImDrawList)

For direct drawing, like rendering overlays, primitives, and graphs, you use:

```cpp
ImDrawList* draw = ImGui::GetBackgroundDrawList();
```

You can also use:

```cpp
    ImGui::GetForegroundDrawList()
    ImGui::GetWindowDrawList() // — to draw inside a specific window
```

# 🖌️ What You Can Draw with ImDrawList

Here’s a list of primitives and shapes you can draw manually:
| Primitive	        | Function                                          |
|------------------ | ------------------------------------------------- |
| Text	            | `AddText(pos, color, "Hello")`                    |   
| Line	            | `AddLine(p1, p2, color, thickness)`               |       
| Rect	            | `AddRect(p_min, p_max, color, rounding)`          |           
| Filled Rect       | `AddRectFilled(...)`                              |
| Circle	        | `AddCircle(center, radius, color)`                |     
| Filled  Circle    | `AddCircleFilled(...)`                            |
| Triangle	        | `AddTriangle(p1, p2, p3, color)`                  |    
| Filled Triangle   | `AddTriangleFilled(...)`                          |
| Quad	            | `AddQuad(...)`                                    |
| Filled Quad       | `AddQuadFilled(...)`                              |
| Polygon	        | `AddPolyline(points[], count, color)`             |        
| Filled Polygon    | `AddConvexPolyFilled(...)`                        |
| Image	            | `AddImage(tex_id, p_min, p_max, uv_min, uv_max)`  |
| Image  with Tint	| `AddImageQuad(...)`                               |

These all go into a draw list and are rasterized with the rest of ImGui’s rendering.
### 🧼 Example: Drawing a Line + Circle + Text
```cpp
ImDrawList* draw = ImGui::GetBackgroundDrawList();
ImVec2 p1 = ImVec2(100, 100);
ImVec2 p2 = ImVec2(200, 200);

draw->AddLine(p1, p2, IM_COL32(255, 0, 0, 255), 2.0f);
draw->AddCircleFilled(ImVec2(150, 150), 30.0f, IM_COL32(0, 255, 0, 255));
draw->AddText(ImVec2(120, 120), IM_COL32_WHITE, "Custom Overlay");
```

**Note:** Coordinates are in screen space, not world space.
### 💡 When to Use What
| Use Case                              | Use ImGui Widget  | Use ImDrawList |
| ------------------------------------- | ----------------- | -------------- |
| Standard UI like sliders/buttons	    | ✅	               | ❌            |
| Game debug overlays (world-space UI)  | ❌	               | ✅            |
| Drawing histograms, graphs manually   | ❌	               | ✅            |
| UI panels with multiple widgets	    | ✅	               | ❌            |
| Drawing texture previews	            | ✅ (Image)        | ✅ (AddImage)  |

Kisses - From GPT