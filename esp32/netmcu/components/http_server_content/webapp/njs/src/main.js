'use strict';
// src/main.js

import App from './wapp.svelte';

export default function () {
  generate_html();
 // appState.init();
}

function generate_html() {
  let isProduction = true;
  // eslint-disable-next-line no-unused-labels 
  testing: { isProduction = false; }

  // eslint-disable-next-line no-unused-vars    
  const app = new App({
    target: document.body,
    props: {
      isProduction: isProduction
    }
  });
}
