<script>
    import {Button, Input, Spinner} from "flowbite-svelte";
    import {ble} from "$lib/ble";
    import TypeContentPacket from "$lib/packet/client/type_content";

    let isConnected = false;
    ble.currentConnection.subscribe(value => {
        isConnected = value?.device?.gatt?.connected ?? false;
    });

    let text = "";

    let isSending = false;
    const send = () => {
        isSending = true;
        const packet = new TypeContentPacket(
            text
        ).toBuffer();
        ble.send(
            packet
        )
            .finally(() => isSending = false);
    }
</script>

<div class="flex flex-col h-full">

    <form on:submit|preventDefault={() => send()}>
        <Input bind:value={text} label="Name" on:submit={() => send()} placeholder="Enter something"/>
    </form>

    <div class="flex-grow"/>

    <Button color="primary" disabled={!isConnected || isSending} on:click={() => send()}>
        {#if (isSending)}
            <Spinner/>
        {:else}
            Send
        {/if}
    </Button>
</div>