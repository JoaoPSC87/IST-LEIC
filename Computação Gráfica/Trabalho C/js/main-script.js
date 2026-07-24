import * as THREE from "three";
import { OBJLoader } from "three/addons/loaders/OBJLoader.js";
import { GLTFLoader } from "three/addons/loaders/GLTFLoader.js";
import { AnaglyphEffect } from "three/addons/effects/AnaglyphEffect.js";

//////////////////////
/* GLOBAL VARIABLES */
//////////////////////

let scene, renderer, camera;
let clock;
let anaglyphEffect;
let anaglyphEnabled = false;
let tesseract;
let animal;
let rocket;
const modelButtons = {};
const lightButtons = {};
const materialButtons = {};
let activeModel = null;
let currentMaterialType = 'lambert';
let lightingEnabled = true;
const textureLoader = new THREE.TextureLoader();

/////////////////////
/* CREATE SCENE(S) */
/////////////////////
function createScene() {
    scene = new THREE.Scene();
    scene.background = new THREE.Color(0x000000); // cor de fundo preta

    createTesseract();
    createAnimal();
    createRocket();

    selectModel('tesseract');

}

//////////////////////
/* CREATE CAMERA(S) */
//////////////////////
function createCamera() {
    const aspect = window.innerWidth / window.innerHeight;
    camera = new THREE.PerspectiveCamera(70, aspect, 0.1, 1000);
    camera.position.set(3, 1, 2);
    camera.lookAt(0, 0, 0);
}
/////////////////////
/* CREATE LIGHT(S) */
/////////////////////
let ambientLight, directionalLight;

function createLights() {
    ambientLight = new THREE.AmbientLight(0xffffee, 0.6);
    scene.add(ambientLight);

    directionalLight = new THREE.DirectionalLight(0xffffff, 1.2);
    directionalLight.position.set(-5, 8, -5);
    directionalLight.target.position.set(0, 0, 0);
    directionalLight.visible = false;
    scene.add(directionalLight);
    scene.add(directionalLight.target);
}

function addModelLights(model) {
    const rig = new THREE.Group();
    model.add(rig);

    const inv = 1 / model.scale.x;
    rig.scale.set(inv, inv, inv);

    const point1 = new THREE.PointLight(0xffffff, 30, 0);
    point1.position.set(5, 5, 5);
    rig.add(point1);

    const point2 = new THREE.PointLight(0xffffff, 30, 0);
    point2.position.set(-5, 3, -5);
    rig.add(point2);

    const spot1 = new THREE.SpotLight(0xffffff, 50, 0, Math.PI / 6, 0.3);
    spot1.position.set(0, 6, 4);
    spot1.target = model;
    rig.add(spot1);

    const spot2 = new THREE.SpotLight(0xffffff, 50, 0, Math.PI / 6, 0.3);
    spot2.position.set(0, -6, 4);
    spot2.target = model;
    rig.add(spot2);

    point1.visible = false; point2.visible = false;
    spot1.visible = false; spot2.visible = false;

    model.userData.pointLights = [point1, point2];
    model.userData.spotLights  = [spot1, spot2];
}

function resetLights() {
    if (activeModel && activeModel.userData.pointLights) {
        for (const light of activeModel.userData.pointLights) {
            light.visible = false;
        }
    }
    if (activeModel && activeModel.userData.spotLights) {
        for (const light of activeModel.userData.spotLights) {
            light.visible = false;
        }
    }
    if (lightButtons['point']) {
        lightButtons['point'].classList.remove('active');
    }
    if (lightButtons['spot']) {
        lightButtons['spot'].classList.remove('active');
    } 
}

function setupMeshMaterials(mesh) {
    const src = Array.isArray(mesh.material) ? mesh.material[0] : mesh.material;
    const props = {
        color:       src.color       ? src.color.clone() : new THREE.Color(0xffffff),
        map:         src.map         || null,
        normalMap:   src.normalMap   || null,
        transparent: src.transparent || false,
        opacity:     src.opacity     !== undefined ? src.opacity : 1,
        side:        src.side        !== undefined ? src.side    : THREE.FrontSide,
    };
    mesh.userData.lambertMat = new THREE.MeshLambertMaterial(props);
    mesh.userData.phongMat   = new THREE.MeshPhongMaterial({ ...props, shininess: 80 });
    mesh.userData.basicMat   = new THREE.MeshBasicMaterial({
        color: props.color, map: props.map,
        transparent: props.transparent, opacity: props.opacity, side: props.side,
    });
}

function applyMaterialMode(model) {
    if (!model) return;
    model.traverse((child) => {
        if (!child.isMesh) return;
        if (!child.userData.lambertMat) setupMeshMaterials(child);
        if (!lightingEnabled) {
            child.material = child.userData.basicMat;
        } else if (currentMaterialType === 'lambert') {
            child.material = child.userData.lambertMat;
        } else {
            child.material = child.userData.phongMat;
        }
    });
}

////////////////////////
/* CREATE OBJECT3D(S) */
////////////////////////

const TESS = {
    outer: 2, // semi-aresta do cubo exterior (vertices em +/- 2)
    inner: 1  // semi-aresta do cubo interior (vertices em +/- 1) 
};

function cubeVertices(s) {
    return [
        [-s, -s, -s],
        [ s, -s, -s],
        [ s,  s, -s],
        [-s,  s, -s],
        [-s, -s,  s],
        [ s, -s,  s],
        [ s,  s,  s],
        [-s,  s,  s], 
    ];
}

function createFace(a, b, c, d, material) {
    const geometry = new THREE.BufferGeometry();

    const vertices = new Float32Array([
        a[0], a[1], a[2],
        b[0], b[1], b[2],
        c[0], c[1], c[2],
        d[0], d[1], d[2],
    ]);

    const indices = [
        0, 1, 2,
        0, 2, 3
    ];

    const uvs = new Float32Array([
        0, 0,
        1, 0,
        1, 1,
        0, 1,
    ]);

    geometry.setAttribute('position', new THREE.BufferAttribute(vertices, 3));
    geometry.setAttribute('uv', new THREE.BufferAttribute(uvs,2));
    geometry.setIndex(indices);
    geometry.computeVertexNormals();

    return new THREE.Mesh(geometry, material);
}

function createCubeFaces(verts, material) {
    const group = new THREE.Group();

    const faces = [
        [4, 5, 6, 7],
        [1, 0, 3, 2],
        [0, 4, 7, 3],
        [5, 1, 2, 6],
        [3, 7, 6, 2],
        [0, 1, 5, 4],
    ];

    for (const f of faces) {
        const face = createFace(verts[f[0]], verts[f[1]], verts[f[2]], verts[f[3]], material);
        group.add(face);
    }
    return group;
}

function createEdge(a, b, material) {
    const start = new THREE.Vector3(a[0], a[1], a[2]);
    const end = new THREE.Vector3(b[0], b[1], b[2]);

    const dir = new THREE.Vector3().subVectors(end, start);
    const length = dir.length();

    const radius = 0.03;
    const geometry = new THREE.CylinderGeometry(radius, radius, length, 8);
    const edge = new THREE.Mesh(geometry, material);

    const mid = new THREE.Vector3().addVectors(start, end).multiplyScalar(0.5);
    edge.position.copy(mid);

    edge.quaternion.setFromUnitVectors(
        new THREE.Vector3(0, 1, 0),
        dir.clone().normalize()
    );

    return edge;
}

function createTesseract() {
    tesseract = new THREE.Group();

    const outerVerts = cubeVertices(TESS.outer);
    const innerVerts = cubeVertices(TESS.inner);

    const normalMap = textureLoader.load('./assets/circles_normal.png');

    const faceMat = new THREE.MeshLambertMaterial({
        color: 0x3399ff,
        transparent: true,
        opacity: 0.4,
        side: THREE.DoubleSide,
        normalMap: normalMap
    });

    const cuboExterior = createCubeFaces(outerVerts, faceMat);
    tesseract.add(cuboExterior);

    const cuboInterior = createCubeFaces(innerVerts, faceMat);
    tesseract.add(cuboInterior);

    const edgeMat = new THREE.MeshLambertMaterial({ color: 0x3399ff });
    const arestas = new THREE.Group();

    for (let i = 0; i < 8; i++) {
        const edge = createEdge(outerVerts[i], innerVerts[i], edgeMat);
        arestas.add(edge);
    }
    tesseract.add(arestas);

    scene.add(tesseract);
    addModelLights(tesseract);
    
}

function createAnimal() {
    const loader = new OBJLoader();
    loader.load(
        './assets/bunny.obj',
        function (object) {
            const mat = new THREE.MeshLambertMaterial({ color: 0xffffff });

            object.traverse((child) => {
                if (child.isMesh) {
                    child.material = mat;
                }
            });

            animal = object;
            animal.visible = false;
            animal.scale.set(1, 1, 1);
            animal.position.set(0, -0.5, 0);
            scene.add(animal);

            addModelLights(animal);
        },
        undefined,
        function (error) {
            console.error('Erro ao carregar modelo:', error);
        }
    );
}

function createRocket() {
    const loader = new GLTFLoader();
    loader.load(
        './assets/nasa_sls_rocket_block_1.glb',
        function (gltf) {
            rocket = gltf.scene;

            const box = new THREE.Box3().setFromObject(rocket);
            const centre = new THREE.Vector3();
            const size = new THREE.Vector3();
            box.getCenter(centre);

            box.getSize(size);
            const maxDim = Math.max(size.x, size.y, size.z);
            const s = 3 / maxDim;
            rocket.scale.set(s, s, s);

            rocket.position.set(-centre.x * s, -centre.y * s, -centre.z * s);

            rocket.visible = false;
            scene.add(rocket);
            addModelLights(rocket);
        },
        undefined,
        function (error) {
            console.error('Erro ao carregar modelo:', error);
        }
    );
}

/////////
/* HUD */
/////////

function createHUD() {
    const hud = document.createElement('div');
    hud.id = 'hud';

    const panels = [
        {
            id: 'panel-models',
            title: 'Modelos',
            buttons: [
                { label: 'Tesseract', name: 'tesseract', action: () => selectModel('tesseract') },
                { label: 'Animal', name: 'animal', action: () => selectModel('animal') },
                { label: 'Artemis', name: 'artemis', action: () => selectModel('artemis') },
            ]
        },
        {
            id: 'panel-effects',
            title: 'Efeitos',
            buttons: [
                { label: 'Anaglífico', action: (btn) => toggleAnaglyph(btn) },

            ]
        },
        {
            id: 'panel-lights',
            title: 'Luzes',
            buttons: [
                { label: 'Direcional', lightType: 'directional', action: (btn) => toggleLight('directional', btn) },
                { label: 'Pontuais', lightType: 'point', action: (btn) => toggleLight('point', btn) },
                { label: 'Spotlight', lightType: 'spot', action: (btn) => toggleLight('spot', btn) },
            ]
        },
        {
            id: 'panel-materials',
            title: 'Materiais',
            buttons: [
                { label: 'Gouraud', materialType: 'lambert', action: () => setMaterial('lambert') },
                { label: 'Phong',   materialType: 'phong',   action: () => setMaterial('phong') },
                { label: 'Iluminação', materialType: 'lighting', action: () => toggleLighting() },
            ]
        },
    ];

    for (const panelDef of panels) {
        const panel = document.createElement('div');
        panel.className = 'hud-panel';
        panel.id = panelDef.id;

        const h3 = document.createElement('h3');
        h3.textContent = panelDef.title;
        panel.appendChild(h3);

        for (const btnDef of panelDef.buttons) {
            const btn = document.createElement('button');
            btn.className = 'hud-btn';
            btn.textContent = btnDef.label;
            btn.addEventListener('click', () => btnDef.action(btn));
            panel.appendChild(btn);

            if (btnDef.name) {
                modelButtons[btnDef.name] = btn;
            }

            if (btnDef.lightType) {
                lightButtons[btnDef.lightType] = btn;
            }

            if (btnDef.materialType) {
                materialButtons[btnDef.materialType] = btn;
            }
        }

        hud.appendChild(panel);

    }

    document.body.appendChild(hud);

    materialButtons['lambert']?.classList.add('active');
    materialButtons['lighting']?.classList.add('active');

}

function selectModel(name)   { 
    if (tesseract) tesseract.visible = false;
    if (animal) animal.visible = false;
    if (rocket) rocket.visible = false;

    if (name === 'tesseract' && tesseract) { tesseract.visible = true; activeModel = tesseract; }
    if (name === 'animal' && animal) {animal.visible = true; activeModel = animal;}
    if (name === 'artemis' && rocket) {rocket.visible = true; activeModel = rocket;}

    applyMaterialMode(activeModel);

    for (const key in modelButtons) {
        modelButtons[key].classList.toggle('active', key === name);
    }

    resetLights();

 }
function toggleAnaglyph(btn)    {
    anaglyphEnabled = !anaglyphEnabled;
    btn.classList.toggle('active');
 }
function toggleLight(type, btn)   {
    if (type === 'directional') {
        directionalLight.visible = !directionalLight.visible;
    }
    else if (type === 'point') {
            if (activeModel && activeModel.userData.pointLights) {
                for (const light of activeModel.userData.pointLights) {
                    light.visible = !light.visible;
                }
            }
        
    }
    else if (type === 'spot') {
            if (activeModel && activeModel.userData.spotLights) {
                for (const light of activeModel.userData.spotLights) {
                    light.visible = !light.visible;
                }
            }  
    }
    btn.classList.toggle('active');
}

function setMaterial(type) {
    currentMaterialType = type;
    applyMaterialMode(activeModel);
    materialButtons['lambert']?.classList.toggle('active', type === 'lambert');
    materialButtons['phong']?.classList.toggle('active', type === 'phong');
}

function toggleLighting() {
    lightingEnabled = !lightingEnabled;
    applyMaterialMode(activeModel);
    materialButtons['lighting']?.classList.toggle('active', lightingEnabled);
}


////////////
/* UPDATE */
////////////
function update() {
    const dt = clock.getDelta();

    if (tesseract) {
        const rotSpeed = 0.5;
        tesseract.rotation.y += rotSpeed * dt;
        tesseract.rotation.x += rotSpeed * 0.5 * dt;
        tesseract.rotation.z += rotSpeed * 0.3 * dt; 

        const t = clock.getElapsedTime();
        const scale = 0.5 * (1 + 0.2 * Math.sin(t));
        tesseract.scale.set(scale, scale, scale);
    }

    if (animal) {
        animal.rotation.y += 0.5 * dt;
    }

}

/////////////
/* DISPLAY */
/////////////
function render() {

    if (anaglyphEnabled) {
        anaglyphEffect.render(scene, camera);
    } else {

        renderer.render(scene, camera);
    }
}

////////////////////////////////
/* INITIALIZE ANIMATION CYCLE */
////////////////////////////////
function init() {
    renderer = new THREE.WebGLRenderer({ antialias: true});
    renderer.setSize(window.innerWidth, window.innerHeight);
    document.body.appendChild(renderer.domElement);

    anaglyphEffect = new AnaglyphEffect(renderer);
    anaglyphEffect.setSize(window.innerWidth, window.innerHeight);

    clock = new THREE.Clock();

    createScene();
    createLights();
    createCamera();
    createHUD();

    selectModel('tesseract');

    window.addEventListener('resize', onResize);
}

/////////////////////
/* ANIMATION CYCLE */
/////////////////////
function animate() {
    requestAnimationFrame(animate);
    update();
    render();
}

////////////////////////////
/* RESIZE WINDOW CALLBACK */
////////////////////////////
function onResize() {
    renderer.setSize(window.innerWidth, window.innerHeight);
    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();

    anaglyphEffect.setSize(window.innerWidth, window.innerHeight);
}


init();
animate();