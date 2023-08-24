<script>
    import TextInput from "../components/TextInput.svelte";
    import {Button, Spinner} from "flowbite-svelte";
    import {ble} from "$lib/ble";
    import TypeContentPacket from "$lib/packet/client/type_content";

    let isConnected = false;
    ble.currentConnection.subscribe(value => {
        isConnected = value?.device?.gatt?.connected ?? false;
    });


    let isSending = false;
    const send = () => {
        isSending = true;
        const packet = new TypeContentPacket(
            "Hello World!"
        ).toBuffer();
        ble.send(
            packet
        )
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