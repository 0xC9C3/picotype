<script lang="ts">
    import {ble} from "$lib/ble.ts";
    import {Select} from "flowbite-svelte";
    import {onMount} from "svelte";
    import {log} from "$lib/log";
    import store from "$lib/store";

    let devices = [];
    let selected = null;

    onMount(() => {
        ble.getDevices().then((d) => {
            devices = d;
            log.l(devices);

            // @todo try select last used device from settings when opening the app
        });
    });

    const connect = async () => {
        const device = devices.find((d) => d.id === selected);
        if (!device) {
            return;
        }

        store.loading.set(true);
        ble.connect(device)
            .then((d) => {
                log.l(d);
            })
            .catch((e) => {
                log.e(e);
                store.toast.set({
                    message: e.message,
                    type: "error"
                });

                console.log(selected)
                selected = null;
            })
            .finally(() => {
                store.loading.set(false);
            });
    }

    $: options = devices.map((d) => {
        return {
            name: d.name,
            value: d.id
        }
    });


    ble.currentConnection.subscribe((currentConnection) => {
        selected = currentConnection?.device?.id;
    });
</script>

<div class="w-full">
    <Select bind:value={selected} items={options} label="Select" on:change={() => connect()}
            placeholder="Select an option"/>
</div>