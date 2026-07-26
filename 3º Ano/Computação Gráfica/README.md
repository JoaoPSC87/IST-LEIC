# Computação Gráfica — DroneWatch & Pepper's Ghost

Interactive 3D graphics projects for **Computação Gráfica** (Computer Graphics) at Instituto Superior Técnico — **2025/26**, group **G38**. Built with **Three.js** (WebGL), extended with a **Snap Lens Studio** AR lens and a physical **Pepper's Ghost** holographic display.

**Grades (lab component): Trabalho A 1.5/2 · Trabalho B 3.28/4 · Trabalho C 3.34/4.**

## Team
- João Carvalho — ist 57175
- Gonçalo Nogueira - ist 109485
- Gonçalo Melo - ist 110488

## The three deliveries

### Trabalho A — Design sketches
Pen-and-paper design of the scenes, done before any code: for each key object (the DroneWatch and the Pepper's Ghost models), a scene sketch (overview + node-hierarchy diagram), simple technical drawings (canonical views with dimensions and degrees of freedom), and a scene graph.

### Trabalho B — DroneWatch (`Trabalho_B/`)
An interactive Three.js scene around the *DroneWatch*, a speculative smartwatch–quadcopter hybrid.
- **Six cameras** — three orthographic (top / side / front), a fixed orthographic and a fixed perspective from an off-axis point, and a mobile perspective camera mounted on the watch's edge. Switch with keys `1`–`6`; toggle the camera helpers with `H`.
- **Articulated model** built from primitives only (cylinders, cubes, toruses): four rotor frames with deployment mechanisms and propellers, the watch body, an on-board camera, a takeoff button, and an imported wristband.
- **Balloons** — red low-poly balloons (body, knot, ribbon) placed randomly above the wrist.
- **Flight controls** — `Q` extends/retracts the rotors (the propellers then spin); `A/D`, `W/S`, `U/J` translate; `I/K` yaw and `O/L` pitch. Constant-velocity motion supporting simultaneous keys and clamped degrees of freedom.
- **Collision detection** — hand-implemented sphere–sphere tests between rotors and balloons, triggering a pop animation. Wireframe/solid toggle (`7`); a HUD with the active-key map, an FPS counter, and a dat.GUI panel.
- **AR lens** — the DroneWatch is exported to glTF and imported into **Snap Lens Studio** (`dronewatch/`) to show it on the user's wrist through Snapchat.

### Trabalho C — Pepper's Ghost (`Trabalho_C/`)
An interactive Three.js holographic viewer inspired by the 19th-century **Pepper's Ghost** illusion, driven entirely by a click/tap HUD.
- **Three models**, shown one at a time: a **tesseract** built by hand vertex-by-vertex (semi-transparent faces, concentric-circle normal map, rotating with periodic scaling); an imported **bunny** mesh (white material, spinning on its long axis); and the **NASA SLS / Artemis rocket**, segmented into components with hand-made textures (Gimp + Blender).
- **Lighting** — an angled directional light, a low whitish ambient, and two point + two spotlights anchored to each model, all toggleable from the HUD.
- **Materials & shading** — per object `MeshLambertMaterial` and `MeshPhongMaterial`, switchable between Gouraud and Phong shading, with a lighting on/off toggle.
- **Anaglyph** red-cyan 3D rendering toggle, and proper window-resize handling.
- **Physical display** — a real Pepper's Ghost box (a transparent surface at 45° over a smartphone) was built to view the models as a floating hologram; a ~1-minute demo video (`Video_Demonstraçao_Peppers_Ghost.mp4`) shows the effect in action.

Both B and C follow the update/display animation loop and implement collisions, motion and geometry directly — no external libraries or built-in Three.js physics/collision helpers.

## Running the Three.js scenes

Three.js uses ES modules and loads local assets, so serve the folder over HTTP (opening `index.html` directly will not work). From `Trabalho_B/` or `Trabalho_C/`:

```sh
python -m http.server 8000
# then open http://localhost:8000
```
(or the VS Code *Live Server* extension). The AR lens (Trabalho B) opens in **Snap Lens Studio**.

## Stack

Three.js (WebGL) · JavaScript (ES modules) · Snap Lens Studio (AR) · Blender + glTF · Gimp (textures).
