
class Modal {
  constructor(modalSelector) {
    // modalSelector should be a css query string that refers
    // to the base modal element
    this.modal = document.querySelector(modalSelector)
    var closeButton = document.querySelector(`${modalSelector} .close`);


    closeButton.addEventListener('click', (event) => {
      this.close();
    });

    window.addEventListener('click', (event) => {
      if (event.target == this.modal) this.close();
    });

    // set up animation listener

    function animationListener(event) {
      console.log(event);
      switch (event.animationName) {
        case 'fadein':
          event.target.classList.remove('fadein');
          break;
        case 'fadeout':
          event.target.classList.remove('fadeout');
          event.target.style.display = 'none';
          break;
        default:
          return;
      }
    }

    this.modal.addEventListener('animationend', animationListener, false);

    this.open = this.open.bind(this);
    this.close = this.close.bind(this);
  }

  open() {
    this.modal.style.display = 'block';
    this.modal.classList.add('fadein');
  }

  close() {
    this.modal.classList.add('fadeout');
  }
}
