<script>
    import {Button, Input, Label, Spinner} from "flowbite-svelte";
    import {ble} from "$lib/ble";
    import TypeContentPacket from "$lib/packet/client/type_content";
    import store from "$lib/store";
    import {log} from "$lib/log";

    let isConnected = false;
    ble.currentConnection.subscribe(value => {
        isConnected = value?.device?.gatt?.connected ?? false;
    });

    let username = "";
    let password = "";

    let isSending = false;

    const sendText = async (text) => {
        const packet = new TypeContentPacket(text).toBuffer();
        await ble.send(packet);
    }

    const send = async () => {
        isSending = true;

        const textsToType = [
            username,
            password,
        ];
        for (const text of textsToType) {
            try {
                if (text.length === 0) {
                    continue;
                }

                await sendText(text);
            } catch (e) {
                store.toast.set({
                    message: e.message,
                    type: "error"
                });

                log.e(e);
                break;
            }
        }

        isSending = false;
    }
</script>

<div class="flex flex-col h-full">

    <form on:submit|preventDefault={() => send()}>
        <div class="mb-2">
            <Label class="mb-2" for="username">Username / E-Mail</Label>
            <Input autocomplete="username" autocorrect="off" bind:value={username} id="username"
                   placeholder="Username / E-Mail"
                   type="text"/>
        </div>
        <div>
            <Label class="mb-2" for="password">Password</Label>
            <Input autocomplete="password" autocorrect="off" autofocus="autofocus" bind:value={password} id="password"
                   placeholder="•••••••••"
                   type="password"/>
        </div>
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