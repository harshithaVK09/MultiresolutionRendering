# MultiresolutionRendering
This is a Computer Graphics project focused on Multi-Resolution Rendering, developed as part of my MCA coursework using OpenGL and C++.


# Multi-Resolution Rendering for High-Performance Virtual Reality Systems

## 📌 Project Overview
This project demonstrates a **Multi-Resolution Rendering technique** to improve performance in Virtual Reality (VR) systems by reducing unnecessary GPU workload.

Instead of rendering the entire scene in high resolution, the system renders:
- **High resolution in the center** (where the user focuses).
- **Lower resolution at the edges** (peripheral vision).

This significantly improves **FPS (Frames Per Second)** and reduces GPU computation.

---

## 🎯 Objectives

- Improve VR performance using multi-resolution rendering.
- Reduce GPU load and vertex processing.
- Optimize draw calls.
- Maintain visual quality where the user focuses.

---

## 💡 Key Concept

Human eyes focus mainly on the **center region** while peripheral vision is less detailed.  
This project leverages this fact to render different areas at different resolutions.

---

## 🧠 How It Works

1. The screen is divided into **three zones**:
   - Center → High Resolution
   - Left Side → Low Resolution
   - Right Side → Low Resolution

2. Each zone is assigned a different **LOD (Level of Detail)**:
   - LOD 0 → High Quality  
   - LOD 1 → Low Quality  

3. Based on the object's screen position, the appropriate LOD is applied.

4. Shaders visually represent LOD using colors:
   - Red → High resolution
   - Green → Medium resolution
   - Blue → Low resolution

---

## 🚀 Features

✅ Multi-resolution rendering implementation  
✅ Dynamic LOD selection  
✅ GPU workload reduction  
✅ Vertex processing optimization  
✅ FPS performance improvement  
✅ Shader-based visual analysis  

---

## 🖥️ Technologies Used

- **Language:** C++ / OpenGL / WebGL (based on your implementation)  
- **Graphics:** OpenGL Shaders (Vertex & Fragment Shader)  
- **Tools:** GLFW / GLAD / WebGL API  
- **Concepts:** LOD, GPU pipeline, Multiresolution Rendering

---

## 🔧 Shader Description

The project uses:
- **Vertex Shader** → Handles positioning and transformation using the `uMVP` matrix.
- **Fragment Shader** → Colors objects based on their LOD level.

Shader Variables:
- `aPos` → Vertex position
- `uMVP` → Model View Projection matrix
- `uColor` → Color representing LOD level

---

## 📊 Performance Comparison

| Metric | Without Multi-Resolution | With Multi-Resolution |
|--------|--------------------------|-----------------------|
| GPU Load | High | Low |
| Vertex Processing | High | Reduced |
| Draw Calls | Many | Optimized |
| FPS | Low | Improved |

---

## 🧪 Sample Console Output

```text
Object at -0.8 | LOD 1, Vertices: 6
Object at 0.0  | LOD 0, Vertices: 3
Object at 0.8  | LOD 1, Vertices: 6

Total vertices this frame: 12
Total draw calls this frame: 3

