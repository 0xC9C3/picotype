<script lang="ts">
    import {ble} from "$lib/ble";

    let isConnected = false;

    ble.currentConnection.subscribe((value) => {
        isConnected = value?.device?.gatt?.connected ?? false;
    });

</script>

{#if isConnected}
    <span class="bg-green-600 w-5 h-4 rounded-full"></span>
{:else}
    <span class="bg-red-600 w-5 h-4 rounded-full"></span>
{/if}
