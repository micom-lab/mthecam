#pragma once
// MWUI1-1.h
// holds HTML-doc index.html in progmem
// OM 200301

const char* _uidoc PROGMEM = R"=====(    
  
<!DOCTYPE html>
<html lang="de">
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>MTheCam</title>
    <style>
      * {
        margin: 0;
        margin-top: 5px;
        padding: 0;
        font-weight: normal
      }
      #error {
        color:red;
      }
      #canvas {
        border: solid 1px #606060;
        border-radius: 15px;
        width: 100%;        
      }
      body {
        font: normal 12px "Lucida Console", monospace;
      }
      h1 {
        font-size: 36px;
        font-weight: bold
      }
      h2 {
        font-size: 17px;
      }
      .pageWrapper {
        min-width: 320px;
        width: 450px;
        max-width: 50%;
        margin: 0 auto
      }
      .row0 {
        display: flex;
        flex-direction: row;
        flex-wrap: wrap;
        width: 100%;
        margin: 3px 0px;                
      }
      .row {
        display: flex;
        flex-direction: row;
        flex-wrap: wrap;
        width: 100%;
        height: 30px; 
        margin-top: 1px;                
      }
      .column {
        display: flex;
        flex-direction: column;
        /* flex-basis: 100%;  */
        flex: 1;       
        justify-content: center; 
        /* border: solid 1px red;  */
      }
      .column0 {          
          flex-basis: 28px;       
      }      
      .buttons {
        display: flex;
        flex-grow: 1;
        justify-content:flex-end; 
        align-items: center;  
      }
      .button {
        border-radius: 15px;
        font-size: 15px;
        width: 50%;
        height: 80%;        
        display: block;        
        margin: 15px 4px; 
      }
      .btn-text {
        background-color: rgb(164, 247, 195);
      }
      .btn-image {
        background-color: yellow;
      }
      .range {
          width: 90%;
      }
  </style>
  </head>
<body>
  <div class="pageWrapper">
    <div class="row0">
      <div class="column">
        <h1>MTheCam</h1>
        <h2>Mini Thermo Cam</h2>
      </div>            
      <div class="column">
        <div class="buttons">
            <button class="button btn-text" onclick="setMode('text')">Values</button>            
            <button class="button btn-image" onclick="setMode('image')">Image</button>
        </div>
      </div>      
    </div>    
    <div class="row">
      <div class="column column0">Temperature</div>
      <div class="column" id="tmin">...</div>
      <div class="column" id="tmax">...</div>
      <div class="column" id="tavg">...</div>
    </div>
    <div class="row">
      <div class="column column0">Colormap</div>
      <div class="column" id="cmin">...</div>
      <div class="column" id="cmax">...</div>
      <div class="column" id="batv">...</div>
    </div>
    <div class="row">
      <div class="column column0">Update</div>    
      <div class="column" id="refreshLabel">...
        <!-- label for="refreshSlider" id="refreshLabel">...</label -->    
      </div>      
      <div class="column">
        <input type="range" class="range" name="refreshSlider" id="refreshSlider" min="200" max="3000" value="1000" step="100" onchange="refresh()">
      </div>  
      <div class="column">        
       <label for="toggleGrid">
        <input type="checkbox" name="toggleGrid" onchange="toggleGrid()">
       Grid</label>
      </div>    
    </div>
    <div class="row">
      <div class="column" id="error"></div>
    </div>
    <div class="row">
      <div class="column">
        <canvas id="canvas"></canvas>
      </div>
    </div>    
  </div>
 </div>
 <script>
    let refreshRate     = 1000         // 1000ms refresh-rate
    let mode            = 0
    let refresher       = undefined
    let grid            = false
    const url           = '/frame';  // 'http://192.168.4.30/frame'
    const gridSizeLo    =  8       // frame 8*8 pixel
    const rectSizeLo    = 50       // colored rect for 8 display
    const gridSizeHi    = 32       // frame 32*32 interpolated pixel
    const rectSizeHi    = 12       // colored rect for 32 display
    const tmin          = document.getElementById("tmin")
    const tmax          = document.getElementById("tmax")
    const tavg          = document.getElementById("tavg")
    const cmin          = document.getElementById("cmin")
    const cmax          = document.getElementById("cmax")
    const batv          = document.getElementById("batv")
    const refreshLabel  = document.getElementById("refreshLabel")
    const refreshSlider = document.getElementById("refreshSlider")
    const errorText     = document.getElementById("error")
    const canvas        = document.getElementById("canvas")
    const ctx           = canvas.getContext("2d")

    window.onload = function () {
      resizeCanvas()
      getData();
      refresh()
    }
    function refresh (rate = 1000) {
      if(refresher) {
        clearTimeout(refresher)
        refresher = undefined
      }
      rate = parseInt(refreshSlider.value) || 1000
      refreshLabel.innerHTML = `${rate} ms`
      refresher = setTimeout(function () {
        getData()
        }, rate)
    }
    function padZero (value = '') {
      return value.toString().padStart(4, '0')
    }
    function toggleGrid () {
      grid = !grid
    }
    function infoText (jsonData) {
      errorText.innerHTML = ''
      tmin.innerHTML = `min ${padZero(jsonData.tmin)}`
      tmax.innerHTML = `max ${padZero(jsonData.tmax)}`
      tavg.innerHTML = `avg ${padZero(jsonData.tavg)}`
      cmin.innerHTML = `min ${padZero(jsonData.tminrange)}`
      cmax.innerHTML = `max ${padZero(jsonData.tmaxrange)}`
      batv.innerHTML = `Bat ${padZero(jsonData.batvolt)}mV`
    }
    function getData () {
      fetch(url)
        .then(response => response.json())
        .then(jsonData => {
          if (mode == 1) {
            // tempvalues, colors (interpolated)
            paint(jsonData.frame, jsonData.framecolinter);
          } else {
            // tempvalues, colors (original)
            paint(jsonData.frame, jsonData.framecol)
          }
          infoText(jsonData)
        }).catch(err => {
          errorText.innerHTML = `Error: ${err}`
      }).then(function () {
        refresh()
      })
    }
    // ?
    // function config () {
    //     location = "/config";           // todo....
    //     //mode = 2;
    // }
    function setMode (displayMode = 'text') {
      if (displayMode == 'text') {
        mode = 0
      } else {
        mode = 1
      }
      resizeCanvas()
    }
    function resizeCanvas () {
      if (mode == 0) {
        gridSize = gridSizeLo
        rectSize = rectSizeLo
      } else {
        gridSize = gridSizeHi
        rectSize = rectSizeHi
      }
      canvasSize = rectSize * gridSize
      canvas.width = canvasSize
      canvas.height = canvasSize
    }
    function paint(values, colors) {
      let col = 0, row = 0
      ctx.clearRect(0, 0, canvas.width, canvas.height);
      colors.forEach((color, index) => {
        if ((index % gridSize == 0) && (index > 0)) {
          // begin new row every gridSize values
          row++
          col = 0
        }
        x = col * rectSize
        y = row * rectSize
        ctx.beginPath()
        ctx.rect(x, y, rectSize, rectSize)
        ctx.fillStyle = `hsl(${color}, 100%, 50%)`
        if (grid) {
          ctx.stroke()
        }
        ctx.fill()
        ctx.closePath()
        if (mode == 0) {
          // tempvalues @ 8*8 only
          drawText(values[index], x + 7, y + rectSize / 2)          
        }
        col++
      })
    }
    function drawText (text, x, y) {
      ctx.font = "normal 12px Lucida Console,monospace"
      ctx.fillStyle = "#101010"
      ctx.textBaseline = "middle"
      ctx.fillText(text, x, y)
    }
 </script>
</body>
</html>
)=====";