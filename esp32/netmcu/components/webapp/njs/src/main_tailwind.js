'use strict';

import App from './main_tailwind.svelte';
import * as connWs from './net/conn_ws.js';

export default function () {
  generate_html();
  setTimeout(() => { connWs.websocket(); }, 1000);
}

function generate_html() {
  // eslint-disable-next-line no-unused-vars    
  const app = new App({
    target: document.body,
    props: {
      
    }
  });
}
