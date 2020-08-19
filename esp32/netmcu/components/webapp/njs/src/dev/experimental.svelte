<script>
  import * as pbuf from "../pbuf/mcu_com_pb.js";
  import * as b64 from "js-base64";
  import LPHs from "./lphs.svelte";

  $: lphs = [];


  function test_pb() {
  let buf = b64.toUint8Array("CJgMCF0IxQYI2gkI6AcI6AcI6AcI6AcI6AcI/wcI6AcIrwEIAAgAGA4=");
  let msg = pbuf.ZoneData.deserializeBinary(buf);
  console.log(msg.array);
  console.log(msg.getLphList());
  lphs = msg.getLphList();
  return msg;
}
</script>

<button type="button" on:click={test_pb}>Test PB</button>

<table>
{#each lphs as lph }
<tr><td>{lph}</td></tr>
{/each}
</table>

<LPHs />