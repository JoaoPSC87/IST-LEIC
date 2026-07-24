import * as THREE from "three";
import { OrbitControls } from "three/addons/controls/OrbitControls.js";
import { VRButton } from "three/addons/webxr/VRButton.js";
import Stats from "stats.js";
import { GUI } from "three/addons/libs/lil-gui.module.min.js";
import { STLLoader } from 'three/addons/loaders/STLLoader.js';
import { GLTFExporter } from 'gltfexporter';

//////////////////////
/* GLOBAL VARIABLES */
//////////////////////
let scene, renderer;
let clock;
let ambientLight;
let dirLight;
const cameras = {};
const cameraHelpers = {};
const axesHelpers = [];
let activeCamera;
let helpersVisible = true;
let smartwatch;
let wrist;
let droneWatchSystem;
let balloons = [];
const keys = {};
let SPEED = 5;
let MOV_SPEED = 5;
let ROT_SPEED = 1;
let helixSpeed = 10; 

let armState = 'retracted'; // 'retracted', 'extending', 'extended', 'retracting'
let armOffset = 0; // 0 (retracted) to 1 (extended)
const armSpeed = 1; // unidades por segundo para extensão/retração do braço
const armExtensions = []; // para os 4 grupos de braços
const helixGroups = []; // para os 4 grupos de hélices
const rotorGroups = []; // para os 4 grupos de rotores (para colisões)

let inputLocked = false; // bloqueia teclas enquanto há uma colisão a ser tratada
const exploding = []; // balões que estão em processo de "explosão" (animação de desaparecimento)
let stats;
let wireframeMode = true;

const FRUSTUM_SIZE = 30;

///////////////////
/* GLTF Exporter */
//////////////////

function exportDroneWatch() {
    const exporter = new GLTFExporter();
    exporter.parse(
        droneWatchSystem,
        (result) => {
            const blob = new Blob(
                [JSON.stringify(result, null, 2)],
                { type: "text/plain" }
            );
            const link = document.createElement("a");
            link.href = URL.createObjectURL(blob);
            link.download = "dronewatch.gltf";
            link.click();
            URL.revokeObjectURL(link.href);
        },
        (error) => { console.error("Erro ao exportar GLTF:", error); },
        { binary: false}
    );
}

/////////
/* HUD */
/////////
const keyMap = new Map();
const CAMERA_NAMES = { '1': '1 - Top', '2': '2 - Side', '3': '3 - Front', '4': '4 - Ortho', '5': '5 - Persp', '6': '6 - Mobile' };
let statusCamera, statusArms, statusInput;

function createHUD() {
    const keyDefs = [
        { key: '1', desc: 'Top camera' },    { key: '2', desc: 'Side camera' },
        { key: '3', desc: 'Front camera' },  { key: '4', desc: 'Global ortho' },
        { key: '5', desc: 'Global persp' },  { key: '6', desc: 'Mobile camera' },
        { key: '7', desc: 'Wireframe' },     { key: 'H', desc: 'Toggle helpers' },
        { key: 'Q', desc: 'Extend/retract arms' }, { key: null, desc: '' },
        { key: 'W', desc: 'Move Y+' },       { key: 'S', desc: 'Move Y-' },
        { key: 'A', desc: 'Move X-' },       { key: 'D', desc: 'Move X+' },
        { key: 'U', desc: 'Move Z-' },       { key: 'J', desc: 'Move Z+' },
        { key: 'I', desc: 'Yaw left' },      { key: 'K', desc: 'Yaw right' },
        { key: 'O', desc: 'Pitch up' },      { key: 'L', desc: 'Pitch down' },
    ];

    const hud = document.createElement('div');
    hud.id = 'hud';

    const title = document.createElement('div');
    title.id = 'hud-title';
    title.textContent = 'DroneWatch HUD';
    hud.appendChild(title);

    const statusRow = document.createElement('div');
    statusRow.id = 'hud-status';
    [['status-camera', 'Camera', '5 - Persp'],
     ['status-arms',   'Arms',   'Retracted'],
     ['status-input',  'Input',  'Locked']
    ].forEach(([id, label, value]) => {
        const box = document.createElement('div');
        box.className = 'status-box';
        box.innerHTML = `<div class="status-label">${label}</div>
                         <div class="status-value" id="${id}">${value}</div>`;
        statusRow.appendChild(box);
    });
    hud.appendChild(statusRow);

    const grid = document.createElement('div');
    grid.id = 'hud-grid';
    keyDefs.forEach(({ key, desc }) => {
        const cap = document.createElement('div');
        if (key) {
            cap.className = 'key';
            cap.dataset.key = key;
            cap.textContent = key;
            if (!keyMap.has(key)) keyMap.set(key, []);
            keyMap.get(key).push(cap);
        } else {
            cap.className = 'hud-spacer';
        }
        const label = document.createElement('div');
        label.className = 'hud-desc';
        label.textContent = desc;
        grid.appendChild(cap);
        grid.appendChild(label);
    });
    hud.appendChild(grid);
    document.body.appendChild(hud);

    statusCamera = document.getElementById('status-camera');
    statusArms   = document.getElementById('status-arms');
    statusInput  = document.getElementById('status-input');
}

function hudSetCamera(k) { statusCamera.textContent = CAMERA_NAMES[k]; }
function hudSetArms(state) { statusArms.textContent = state; }
function hudSetInput(ready) { statusInput.textContent = ready ? 'Ready' : 'Locked'; }

/////////////////////
/* CREATE SCENE(S) */
/////////////////////

function addAxesHelper(parent, size = 1) {
    const helper = new THREE.AxesHelper(size);
    parent.add(helper);
    axesHelpers.push(helper);  
    return helper;
}

function createScene() {
    scene = new THREE.Scene();
    scene.background = new THREE.Color(0x333333); // background cinzento escuro
    
    addAxesHelper(scene, 10); // eixos de referência

    droneWatchSystem = new THREE.Group();
    scene.add(droneWatchSystem);

    createSmartwatch();
    createWrist();
    createBalloons();

    
}

//////////////////////
/* CREATE CAMERA(S) */
//////////////////////
function createOrthographicCamera(x, y, z) {
    const aspect = window.innerWidth / window.innerHeight;
    const cam = new THREE.OrthographicCamera(
        -FRUSTUM_SIZE * aspect / 2,
        FRUSTUM_SIZE * aspect / 2,
        FRUSTUM_SIZE / 2,
        -FRUSTUM_SIZE / 2,
        0.1, 1000
    );
    cam.position.set(x, y, z);
    cam.lookAt(0, 0, 0);
    return cam;
}

function createPerspectiveCamera(x, y, z){
    const aspect = window.innerWidth / window.innerHeight;
    const cam = new THREE.PerspectiveCamera(60, aspect, 0.1, 1000);
    cam.position.set(x, y, z);
    cam.lookAt(0, 0, 0);
    return cam;
}

function createCameras(){
    cameras.top = createOrthographicCamera(0, 50, 0); // '1' - vista de topo
    cameras.side = createOrthographicCamera(50, 0, 0); // '2' - vista lateral
    cameras.front = createOrthographicCamera(0, 0, 50); // '3' - vista frontal

    const offAxis = new THREE.Vector3(30, 30, 30);
    cameras.fixedOrtho = createOrthographicCamera(offAxis.x, offAxis.y, offAxis.z); // '4' - vista ortográfica fixa
    cameras.fixedPersp = createPerspectiveCamera(offAxis.x, offAxis.y, offAxis.z); // '5' - vista perspectiva fixa

    const mobile = new THREE.PerspectiveCamera(70, window.innerWidth / window.innerHeight, 0.1, 1000);
    mobile.position.set(0, DIM.body.h / 2, -DIM.body.d /2);
    mobile.lookAt(0, DIM.body.h / 2, -100);
    smartwatch.add(mobile);
    cameras.mobile = mobile; // '6' - vista móvel (acoplada ao watchdrone)

    //cameraHelper para cada câmara
    for (const key in cameras){
        const helper = new THREE.CameraHelper(cameras[key]);
        cameraHelpers[key] = helper;
        scene.add(helper);
    }
    activeCamera = cameras.fixedPersp; // camara ativa inicial (vista perspectiva)
    updateHelpers();
}

function updateHelpers() {
    for (const key in cameraHelpers) {
        cameraHelpers[key].visible = helpersVisible && cameras[key] !== activeCamera;
    }

    for (const helper of axesHelpers) {
        helper.visible = helpersVisible;
    }
}
/////////////////////
/* CREATE LIGHT(S) */
/////////////////////
function createLights() {
    ambientLight = new THREE.AmbientLight(0xffffff, 1);
    scene.add(ambientLight);

    dirLight = new THREE.DirectionalLight(0xffffff, 2);
    dirLight.position.set(10, 20, 10);
    dirLight.lookAt(0, 0, 0);
    scene.add(dirLight);
}
////////////////////////
/* CREATE OBJECT3D(S) */
////////////////////////
const DIM = {
    body: { d: 4, h: 1},  //corpo cilíndrico: diâmetro 4 e altura 1
    camera: { d: 0.5, h: 0.2}, //câmara incorporada
    button: { w: 0.5, h: 0.4, d: 0.5}, //botão largo
    arm: { w: 2, h: 0.5, d: 1}, //braço (extensível)
    rotor: {
        torus: { R: 1.5, r: 0.4 }, //caixilharia
        center: { d: 0.5, h: 0.3 }, //cilindro central da hélice
        blade: { w: 1.2, h: 0.1, d: 0.3 } // cada pá da hélice
    },
    strap: { w: 4, h: 0.3, d: 10}, //pulseira (cada lado, simplificada)
    platform: { h: 0.2}, //plataforma de lançamento
    balloon: {
        body:  {d: 1.5}, //diametro do corpo
        knot: {d: 0.5, h: 0.5}, //diametro e alura do nó
        ribbon: {d: 0.05, h: 2.5} //diametro e altura da fita
    },
    collision: {
        rotorRadius: 1.7, // raio de colisão do rotor (deteção de colisões)
        balloonRadius: 0.95 // raio de colisão do balão (detecção de colisões)
    }
};

function createBody(parent){
    const geo = new THREE.CylinderGeometry(
        DIM.body.d / 2, //raio do topo
        DIM.body.d / 2, //raio da base
        DIM.body.h, //altura
        16 //número de segmentos radiais
    );
    const mat = new THREE.MeshLambertMaterial({color: 0x368f8b, wireframe: true});
    const body = new THREE.Mesh(geo, mat);
    body.position.set(0,0,0);
    addAxesHelper(body, 2); //eixos locais (debug)
    parent.add(body);
}

function createWatchCamera(parent){
    const geo = new THREE.CylinderGeometry(DIM.camera.d / 2 , DIM.camera.d / 2, DIM.camera.h, 16);
    const mat = new THREE.MeshLambertMaterial({color: 0x160f29, wireframe: true});
    const camera = new THREE.Mesh(geo, mat);
    camera.position.set(0, DIM.body.h / 2 + DIM.camera.h / 2, -3*DIM.body.d / 8);
    addAxesHelper(camera, 0.5);
    parent.add(camera);
}

function createButton(parent){
    const geo = new THREE.BoxGeometry(DIM.button.w, DIM.button.h, DIM.button.d);
    const mat = new THREE.MeshLambertMaterial({color: 0xddbea8, wireframe: true});
    const button = new THREE.Mesh(geo, mat);
    button.position.set(0, DIM.body.h / 2 + DIM.button.h / 2, 3*DIM.body.d / 8);
    parent.add(button);
}

function createArm(parent, angle){
    const armGroup = new THREE.Group();
    armGroup.rotation.y = angle;

    const extension = new THREE.Group();
    extension.position.set(0, 0, 0);
    armGroup.add(extension);
    armExtensions.push(extension); // para animar a extensão/retração dos braços

    const armGeo = new THREE.BoxGeometry(DIM.arm.w, DIM.arm.h, DIM.arm.d);
    const armMat = new THREE.MeshLambertMaterial({color: 0xddbea8, wireframe: true});
    const armMesh = new THREE.Mesh(armGeo, armMat);
    armMesh.position.set(DIM.body.d / 2 + DIM.arm.w / 2, 0, 0);
    extension.add(armMesh);

    createRotor(extension);

    addAxesHelper(extension, 2); //eixos locais (debug)
    parent.add(armGroup);
}

function createRotor(parent){
    const rotor = new THREE.Group();
    rotor.position.set(DIM.body.d / 2 + DIM.arm.w, 0, 0);
    rotorGroups.push(rotor); // para detecção de colisões

    const torusGeo = new THREE.TorusGeometry(DIM.rotor.torus.R, DIM.rotor.torus.r, 6, 16);
    const torusMat = new THREE.MeshLambertMaterial({color: 0x368f8b, wireframe: true});
    const torus = new THREE.Mesh(torusGeo, torusMat);
    torus.rotation.x = Math.PI / 2;
    rotor.add(torus);

    const helix = new THREE.Group();
    helix.position.set(0, DIM.rotor.torus.r + DIM.rotor.center.h / 2, 0);
    helixGroups.push(helix); // para animar a rotação das hélices

    const centerGeo = new THREE.CylinderGeometry(
        DIM.rotor.center.d / 2,
        DIM.rotor.center.d / 2,
        DIM.rotor.center.h,
        16
    );
    const centerMat = new THREE.MeshLambertMaterial({color: 0x368f8b, wireframe: true});
    const center = new THREE.Mesh(centerGeo, centerMat);
    helix.add(center);

    for(let i = 0; i < 4; i++){
        const bladeGeo = new THREE.BoxGeometry(
            DIM.rotor.blade.w,
            DIM.rotor.blade.h,
            DIM.rotor.blade.d
        );
        const bladeMat = new THREE.MeshLambertMaterial({color: 0xddbea8, wireframe: true});
        const blade = new THREE.Mesh(bladeGeo, bladeMat);
        blade.position.set(DIM.rotor.blade.w / 2, 0, 0);

        const bladeGroup = new THREE.Group();
        bladeGroup.rotation.y = i * Math.PI / 2;
        bladeGroup.add(blade);
        helix.add(bladeGroup);
    }

    rotor.add(helix);
    addAxesHelper(rotor, 1); //eixos locais (debug)
    parent.add(rotor);
}


function createSmartwatch() {
    smartwatch = new THREE.Group();
    smartwatch.rotation.order = 'YXZ';
    createBody(smartwatch);
    createWatchCamera(smartwatch);
    createButton(smartwatch);

    createArm(smartwatch, Math.PI / 4); // front-right
    createArm(smartwatch, 3 * Math.PI / 4); // front-left
    createArm(smartwatch, -3 * Math.PI / 4);     // back-left
    createArm(smartwatch, -Math.PI / 4);// back-right
    
    droneWatchSystem.add(smartwatch);
}
function createStrap(parent) {
    const loader = new STLLoader();
    loader.load(
        './js/Swatch_Wristband_20mm-test.stl',
        function (geometry) {
            const material = new THREE.MeshLambertMaterial({color: 0x000000, wireframe: true});
            const strap = new THREE.Mesh(geometry, material);

            strap.scale.set(0.09, 0.09, 0.09); // Ajusta a escala do modelo
            strap.rotation.set(Math.PI / 2, Math.PI, Math.PI / 2); // Ajusta a rotação do modelo
            strap.position.set(0, -1.8 * DIM.body.h - DIM.platform.h, 0); // Ajusta a posição do modelo

            addAxesHelper(strap, 2);
            parent.add(strap);

        },
        undefined,
        function(error) {
            console.error('Erro ao carregar o modelo STL:', error);
        }
    );
}
function createWrist() {
    wrist = new THREE.Group();

    const platformGeo = new THREE.CylinderGeometry(
        DIM.body.d / 2, // igual ao corpo do drone
        DIM.body.d / 2, 
        DIM.platform.h, //altura da plataforma 
        16
    );

    const platformMat = new THREE.MeshLambertMaterial({color: 0x160f29, wireframe: true});
    const platform = new THREE.Mesh(platformGeo, platformMat);
    platform.position.set(0, -DIM.body.h / 2 - DIM.platform.h / 2, 0);
    addAxesHelper(platform, 1);
    wrist.add(platform);

    createStrap(wrist);

    droneWatchSystem.add(wrist);
}

function createBalloon(x, y, z) {
    const balloon = new THREE.Group();
    balloon.position.set(x, y, z);

    const bodyGeo = new THREE.SphereGeometry(DIM.balloon.body.d / 2, 8, 8);
    const bodyMat = new THREE.MeshLambertMaterial({color: 0xff0000, wireframe: true});
    const body = new THREE.Mesh(bodyGeo, bodyMat);
    body.position.set(0, 0, 0);
    balloon.add(body);

    const knotGeo = new THREE.ConeGeometry(DIM.balloon.knot.d / 2, DIM.balloon.knot.h, 8);
    const knotMat = new THREE.MeshLambertMaterial({color: 0xaa0000, wireframe: true});
    const knot = new THREE.Mesh(knotGeo, knotMat);
    knot.position.set(0, -DIM.balloon.body.d / 2 - DIM.balloon.knot.h / 2, 0);
    knot.rotation.x = Math.PI;
    balloon.add(knot);

    const ribbonGeo = new THREE.CylinderGeometry(DIM.balloon.ribbon.d / 2, DIM.balloon.ribbon.d / 2, DIM.balloon.ribbon.h, 8);
    const ribbonMat = new THREE.MeshLambertMaterial({color: 0xffffff, wireframe: true});
    const ribbon = new THREE.Mesh(ribbonGeo, ribbonMat);
    ribbon.position.set(0, -DIM.balloon.body.d / 2 - DIM.balloon.knot.h - DIM.balloon.ribbon.h / 2, 0);
    balloon.add(ribbon);

    addAxesHelper(balloon, 1);
    scene.add(balloon);
    balloons.push(balloon);

}

function createBalloons(N = 6) {

    for(let i = 0; i < N; i++){
        const x = (Math.random() - 0.5) * 30; 
        const y = 5 + Math.random() * 15;
        const z = (Math.random() - 0.5) * 30; 
        createBalloon(x, y, z);
    }
}

function resetBalloons(N, scale) {
    for(const b of balloons) scene.remove(b);
    balloons.length = 0;
    exploding.length = 0;
    inputLocked = false;
    createBalloons(N);
    for(const balloon of balloons){
        balloon.scale.set(scale, scale, scale);
    }
    updateHelpers();
    applyWireframe();
}

function applyWireframe(){
    scene.traverse((obj) => {
        if(obj.isMesh) obj.material.wireframe = wireframeMode;
    });
}
//////////////////////
/* CHECK COLLISIONS */
//////////////////////
function checkCollisions() {
    if(inputLocked) return; // não processa novas colisões enquanto uma animação de colisão está em andamento

    const rotorPosition = rotorGroups.map(rotor => {
        const pos = new THREE.Vector3();
        rotor.getWorldPosition(pos);
        return pos;
    });

    for(const balloon of balloons) {
        if(exploding.includes(balloon)) continue; // ignora balões que já estão a rebentar

        const balloonPos = new THREE.Vector3();
        balloon.getWorldPosition(balloonPos);

        for(const rotorPos of rotorPosition) {
            const distance = balloonPos.distanceTo(rotorPos);
            const sumRadii = DIM.collision.rotorRadius + DIM.collision.balloonRadius;

            if(distance < sumRadii) {
                handleCollisions(balloon);
                return; // só processa uma colisão por frame
            }
        }
    }
}


///////////////////////
/* HANDLE COLLISIONS */
///////////////////////
function handleCollisions(balloon) {
    inputLocked = true; // bloqueia novas colisões
    exploding.push(balloon); // marca o balão como "a rebentar"
    balloon.userData.explosionTime = 0; // tempo desde o início da "explosão"
}

////////////
/* UPDATE */
////////////
function update() {
    const dt = clock.getDelta();

    if(armState === 'extending') {
        armOffset += armSpeed * dt;
        if(armOffset >= 1) {
            armOffset = 1;
            armState = 'extended';
            hudSetArms('Extended');
            hudSetInput(true);
        }

    } else if(armState === 'retracting') {
        armOffset -= armSpeed * dt;
        if(armOffset <= 0) {
            armOffset = 0;
            armState = 'retracted';
            hudSetArms('Retracted');
            hudSetInput(false);
        }
    }

    const offsetX = (armOffset - 1) * (DIM.arm.w * 1.8); // deslocamento do braço (0 a -DIM.arm.w)
    for(const extension of armExtensions){
        extension.position.x = offsetX;
    }

    const rotorVisible = armOffset > 0.05;
    for(const rotor of rotorGroups){
        rotor.visible = rotorVisible;
    }

    if(armState === 'extended') {
        for(const helix of helixGroups){
            helix.rotation.y += helixSpeed * dt;
        }
    }

        // Movimentação do drone
    if(armState === 'extended' && !inputLocked) {    
        if (keys['d']) smartwatch.position.x += SPEED * dt;
        if (keys['a']) smartwatch.position.x -= SPEED * dt;
        if (keys['w']) smartwatch.position.y += SPEED * dt;
        if (keys['s']) smartwatch.position.y -= SPEED * dt;
        if (keys['j']) smartwatch.position.z += SPEED * dt;
        if (keys['u']) smartwatch.position.z -= SPEED * dt;

                // Rotação do drone
        if (keys['i']) smartwatch.rotation.y -= ROT_SPEED * dt; // yaw
        if (keys['k']) smartwatch.rotation.y += ROT_SPEED * dt;
        if (keys['o']) smartwatch.rotation.x = Math.min(smartwatch.rotation.x + ROT_SPEED * dt, Math.PI / 4); // pitch
        if (keys['l']) smartwatch.rotation.x = Math.max(smartwatch.rotation.x - ROT_SPEED * dt, -Math.PI / 4);
    }
    checkCollisions();

    for(let i = exploding.length - 1; i >= 0; i--){
        const balloon = exploding[i];
        balloon.userData.explosionTime += dt;

        const duration = 0.5; // duração da "explosão" em segundos
        const progress = balloon.userData.explosionTime / duration;

        if(progress >= 1) {
            scene.remove(balloon);
            balloons.splice(balloons.indexOf(balloon), 1);
            exploding.splice(i, 1);

            if(exploding.length === 0) {
                inputLocked = false; // desbloqueia a entrada quando todas as animações de colisão terminarem
            }
        } else {
            const scale = 1 - progress; // o balão encolhe até desaparecer
            balloon.scale.set(scale, scale, scale);
        }
    }
}

/////////////
/* DISPLAY */
/////////////
function render() {
    renderer.render(scene, activeCamera);
}

///////////
/* GUI */
///////////
function createGUI() {
    const gui = new GUI();
    const params = { numBalloons: 6, balloonScale: 1, droneScale: 1, smartwatchScale: 1 };

    const droneFolder = gui.addFolder('Drone');
    droneFolder.add({ speed: SPEED }, 'speed', 1, 20, 0.5).name('Move speed').onChange(v => SPEED = v);
    droneFolder.add({ rot: ROT_SPEED }, 'rot', 0.1, 5, 0.1).name('Rotation speed').onChange(v => ROT_SPEED = v);
    droneFolder.add({ helix: helixSpeed }, 'helix', 1, 30, 1).name('Helix speed').onChange(v => helixSpeed = v);
    droneFolder.add(params, 'droneScale', 0.5, 2, 0.1).name('Scale Drone').onChange(v => {
        wrist.scale.set(v, v, v);
        smartwatchScaleCtrl.setValue(v);
    });
    const smartwatchScaleCtrl = droneFolder.add(params, 'smartwatchScale', 0.5, 2, 0.1).name('Scale Smartwatch').onChange(v => {
        smartwatch.scale.set(v, v, v);
    });
    droneFolder.add({ export: exportDroneWatch }, 'export').name('Export GLTF');

    const balloonsFolder = gui.addFolder('Balloons');
    balloonsFolder.add(params, 'numBalloons', 1, 20, 1).name('Count');
    balloonsFolder.add(params, 'balloonScale', 0.5, 3, 0.1).name('Scale').onChange(v => {
        for(const balloon of balloons){
            balloon.scale.set(v, v, v);
        }
    });
    balloonsFolder.add({ reset: () => resetBalloons(params.numBalloons, params.balloonScale) }, 'reset').name('Recreate');
}

////////////////////////////////
/* INITIALIZE ANIMATION CYCLE */
////////////////////////////////
function init() {
    stats = new Stats();
    stats.showPanel(0); // 0: fps, 1: ms, 2: mb, 3+: custom
    document.body.appendChild(stats.dom);
    renderer = new THREE.WebGLRenderer({antialias: true});
    renderer.setSize(window.innerWidth, window.innerHeight);
    document.body.appendChild(renderer.domElement);

    clock = new THREE.Clock();

    createScene();
    createCameras();
    createHUD();
    createGUI();
    createLights();

    window.addEventListener('resize', onResize);
    window.addEventListener('keydown', onKeyDown);
    window.addEventListener('keyup', onKeyUp);
}

/////////////////////
/* ANIMATION CYCLE */
/////////////////////
function toggleArms() {
    if(armState === 'retracted' || armState === 'retracting'){
        armState = 'extending';
    } else if (armState === 'extended' || armState === 'extending'){
        armState = 'retracting';
    }
}

function animate() {
    stats.begin();
    requestAnimationFrame(animate);
    update();
    render();
    stats.end();
}

////////////////////////////
/* RESIZE WINDOW CALLBACK */
////////////////////////////
function onResize() {
    renderer.setSize(window.innerWidth, window.innerHeight);
    const aspect = window.innerWidth / window.innerHeight;

    for(const key in cameras) {
        const cam = cameras[key];
        if(cam.isOrthographicCamera){
            cam.left = -FRUSTUM_SIZE * aspect / 2;
            cam.right = FRUSTUM_SIZE * aspect / 2;
            cam.top = FRUSTUM_SIZE / 2;
            cam.bottom = -FRUSTUM_SIZE / 2;
        } else {
            cam.aspect = aspect;
        }
        cam.updateProjectionMatrix();
        cameraHelpers[key].update();
    }
}

///////////////////////
/* KEY DOWN CALLBACK */
///////////////////////
function setActiveCamera(cam) {
    activeCamera = cam;
    updateHelpers();
}
function onKeyDown(e) {
    if(e.repeat) return;
    keys[e.key.toLowerCase()] = true;
    keyMap.get(e.key.toUpperCase())?.forEach(d => d.classList.add('pressed'));
    if(inputLocked) return;
    switch(e.key){
        case '1': setActiveCamera(cameras.top);        hudSetCamera('1'); break;
        case '2': setActiveCamera(cameras.side);       hudSetCamera('2'); break;
        case '3': setActiveCamera(cameras.front);      hudSetCamera('3'); break;
        case '4': setActiveCamera(cameras.fixedOrtho); hudSetCamera('4'); break;
        case '5': setActiveCamera(cameras.fixedPersp); hudSetCamera('5'); break;
        case '6': setActiveCamera(cameras.mobile);     hudSetCamera('6'); break;
        case '7':
            wireframeMode = !wireframeMode;
            applyWireframe();
            break;
        case 'h':
        case 'H':
            helpersVisible = !helpersVisible;
            updateHelpers();
            break;
        case 'q':
        case 'Q':
            toggleArms();
            break;
    }
}

///////////////////////
/* KEY UP CALLBACK */
///////////////////////
function onKeyUp(e) {
    keys[e.key.toLowerCase()] = false;
    keyMap.get(e.key.toUpperCase())?.forEach(d => d.classList.remove('pressed'));
}


init();
animate();