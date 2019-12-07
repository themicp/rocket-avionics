const scene = new THREE.Scene()
scene.background = new THREE.Color(0x555555);
const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000)

const renderer = new THREE.WebGLRenderer();
renderer.shadowMap.enabled = true;

renderer.setSize( window.innerWidth, window.innerHeight );

document.body.appendChild( renderer.domElement );

moto = null
var loader = new THREE.GLTFLoader();

loader.load(
  '/models/motorcycle.gltf',
  (object) => {
    moto = object.scene
    scene.add(moto)
  },
  (xhr) => {
    console.log((xhr.loaded / xhr.total * 100) + '% loaded')
  },
  (error) => {
    console.log( 'An error happened' )
  }
)

const light = new THREE.SpotLight(0xffffff);
light.position.set( 0, 140, 150 );
light.rotation.x = -30 * Math.PI/180
scene.add( light );

camera.position.z = 150
camera.position.y = 140
camera.rotation.x = -30 * Math.PI/180
camera.position.x = 0

function animate() {
	requestAnimationFrame(animate)
	renderer.render(scene, camera)
}
animate()

setRoll = (degrees) => {
  moto.rotation.z = degrees * Math.PI / 180
}

setPitch = (degrees) => {
  //moto.rotation.x = degrees * Math.PI / 180
}

setYaw = (degrees) => {
  moto.rotation.y = degrees * Math.PI / 180
}
