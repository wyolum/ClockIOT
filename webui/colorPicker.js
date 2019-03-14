'use strict';

class ColorPicker extends EventTarget {
  constructor(pickerImg) {
    super();
    // get image width and height
    this.width = pickerImg.width;
    this.height = pickerImg.height;
    function loadCanvas() {
      // create copy of image on canvas
      var canvas = document.createElement('canvas');
      canvas.width = pickerImg.naturalWidth;
      canvas.height = pickerImg.naturalHeight;
      canvas.style.display = 'none';
      document.body.appendChild(canvas);
      this.ctx = canvas.getContext('2d');
      this.ctx.drawImage(pickerImg, 0, 0, 1024, 1024);
      this.ctx.scale(pickerImg.width / pickerImg.naturalWidth, pickerImg.height / pickerImg.naturalHeight);
      // get array of pixels from canvas
      // array is red, green, blue, alpha
      // repeated
      this.pixelArray = this.ctx.getImageData(0, 0, this.width, this.height).data;
    }
    loadCanvas = loadCanvas.bind(this);
    if (pickerImg.naturalWidth === 0) { // if image isn't loaded yet, wait until it is
      pickerImg.addEventListener('load', loadCanvas);
    } else {
      loadCanvas();
    }

    this.pickerListener = this.pickerListener.bind(this);
    pickerImg.addEventListener('click', this.pickerListener);
    pickerImg.addEventListener('mousemove', this.pickerListener);
  }

  pickerListener(event) {
    var posX = event.pageX - event.target.offsetLeft;
    var posY = event.pageY - event.target.offsetTop;
    var color = this.getPixelColor(posX, posY);
    this.mouseColor = color;
    this.dispatchEvent(new CustomEvent(event.type, {detail:
      {
        color: color
      }
    }));
  }

  /**
  * Gets the pixel at x,y and returns the color of it
  * @function getPixelColor
  * @private
  * @param {number} x
  * @param {number} y
  * @returns {string} hex color
  */
  getPixelColor(x, y) {
    var offset = (y * this.width + x)* 4;
    var color = this.pixelArray.slice(offset, offset + 4);
    if (color[4] >= 255) color.pop(); // remove alpha if it's completely opaque
    var hex = '#';
    for (let part of color) {
      hex += part.toString(16).padStart(2, 0);
    }
    return hex;
  }
}
