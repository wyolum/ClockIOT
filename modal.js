
class Modal {
  constructor(modalSelector) {
    // modalSelector should be a css query string that refers
    // to a div
    // build the modal
    this.modal = document.querySelector(modalSelector)
    this.modal.classList.add('modal');

    var container = document.createElement('div');
    container.classList.add('modalContent', 'container');

    var closeContainer = document.createElement('div');
    closeContainer.classList.add('close');
    container.appendChild(closeContainer);

    var closeButton = document.createElement('span');
    closeButton.innerHTML = '&times;';
    closeContainer.appendChild(closeButton);

    var modalContent = document.createElement('div');
    modalContent.innerHTML = this.modal.innerHTML;
    container.appendChild(modalContent);

    this.modal.innerHTML = '';
    this.modal.appendChild(container);

    closeButton.addEventListener('click', (event) => {
      this.close();
    });

    window.addEventListener('click', (event) => {
      if (event.target == this.modal) this.close();
    });

    // set up animation listener

    function animationListener(event) {
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

  openButton() {
    // returns a button that opens modal
    var button = document.createElement('button');
    button.addEventListener('click', (event) => {
      this.open();
    });
    button.innerText = 'Open Modal';
    return button;
  }
}
