<script lang="ts">
    import {Button} from "flowbite-svelte";
    import {ble} from "$lib/ble";
    import {log} from "$lib/log";
    import store from "$lib/store";
    import ConnectionState from "./ConnectionState.svelte";

    const addDevice = () => {
        store.loading.set(true);
        ble.addDevice()
            .then(() => {
                log.l("Device added");
            })
            .catch(error => {
                log.l(error);
                store.toast.set({
                    message: error.message,
                    type: "error"
                });
            })
            .finally(() => {
                store.loading.set(false);
            });
    }

    let currentConnection = null;
    ble.currentConnection.subscribe(value => {
        currentConnection = value;
    });
</script>

<Button class="w-full min-w-0" on:click={() => addDevice()} size="sm">
    <ConnectionState/>

    {#if currentConnection}
        <div class="ml-3 overflow-hidden whitespace-nowrap overflow-ellipsis max-w-full">
            {currentConnection.device.name} - {currentConnection.device.id}
        </div>
    {:else}
        <div class="ml-3">
            Not connected
        </div>
    {/if}
</Button>