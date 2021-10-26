let mode = document.getElementById('toggleBtn')

function changeStatus() {
  let pressureContainer = document.getElementById('sliderPressureContainer');
  let speedContainer = document.getElementById('sliderSpeedContainer');
  console.log(mode.checked)
  if (mode.checked) {
    pressureContainer.hidden = false;
    speedContainer.hidden = true;
  }
  else {
    pressureContainer.hidden = true;
    speedContainer.hidden = false;
  }
}

changeStatus();

// TODO connect the slider to the speed of the animation
fanSpeed = anime({
    targets: '#fanSvg',
    easing: "linear",
    rotate: '1turn',
    loop: true,
    duration: 3000

  });

function updatePressure(pressure){
  document.getElementById('displayPressure').innerHTML = pressure + "Pa"
}

function updateSpeed(speed){
  document.getElementById('displaySpeed').innerHTML = speed + "%";
}