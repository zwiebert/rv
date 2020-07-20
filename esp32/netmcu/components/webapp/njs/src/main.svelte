<script>
  "use strict";
  import Layout from "./layout.svelte";
  import Wapp from "./wapp.svelte";

  export let isProduction = false;
</script>

<Layout />
<Wapp {isProduction} />
