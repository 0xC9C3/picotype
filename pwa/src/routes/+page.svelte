<script>
    import TextInput from "../components/TextInput.svelte";
    import {Button, Spinner} from "flowbite-svelte";
    import {ble} from "$lib/ble";

    let isConnected = false;
    ble.currentConnection.subscribe(value => {
        isConnected = value?.device?.gatt?.connected ?? false;
    });


    let isSending = false;
    const send = () => {
        isSending = true;
        ble.send([0x01, 0x02, 0x03, 0x02, 0x01])
            .finally(() => isSending = false);
    }
</script>

<div class="flex flex-col h-full">


    <TextInput/>

    <div class="flex-grow"/>

    <Button color="primary" disabled={!isConnected || isSending} on:click={() => send()}>
        {#if (isSending)}
            <Spinner/>
        {:else}
            Send
        {/if}
    </Button>
</div>