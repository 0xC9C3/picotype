<script>
    import {Button, ButtonGroup, Input, Label, Spinner} from "flowbite-svelte";
    import {ble} from "$lib/ble";
    import TypeContentPacket from "$lib/packet/client/type_content";
    import store from "$lib/store";
    import {log} from "$lib/log";
    import {Icon} from "flowbite-svelte-icons";

    let isConnected = false;
    ble.currentConnection.subscribe(value => {
        isConnected = value?.device?.gatt?.connected ?? false;
    });

    let username = "";
    let password = "";

    let isSending = false;

    const sendText = async (text) => {
        if (text.length === 0) {
            return;
        }
        try {
            const packet = new TypeContentPacket(text).toBuffer();
            await ble.send(packet);
        } catch (e) {
            store.toast.set({
                message: e.message,
                type: "error"
            });

            log.e(e);
        }
    }

    const send = async () => {
        isSending = true;

        const textsToType = [
            username,
            password,
        ];
        for (const text of textsToType) {
            await sendText(text);

            // @todo send tab between strokes
        }

        isSending = false;
    }

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
</script>

<div class="flex flex-col h-full">

    {#if isConnected}
        <div class="h-full flex flex-col justify-center">
            <form on:submit|preventDefault={() => send()}>
                <div class="mb-3">
                    <Label class="mb-2" for="username">Username / E-Mail</Label>
                    <ButtonGroup class="w-full">
                        <Input autocomplete="username" autocorrect="off" bind:value={username} id="username"
                               placeholder="Username / E-Mail"
                               type="text"/>
                        <Button on:click={() => sendText(username)} color="primary">
                            <Icon name="arrow-right-outline"/>
                        </Button>
                    </ButtonGroup>
                </div>
                <div>
                    <Label class="mb-2" for="password">Password</Label>
                    <ButtonGroup class="w-full">
                        <Input autocomplete="password" autocorrect="off" autofocus="autofocus" bind:value={password}
                               id="password"
                               placeholder="•••••••••"
                               type="password"/>
                        <Button on:click={() => sendText(password)} color="primary">
                            <Icon name="arrow-right-outline"/>
                        </Button>
                    </ButtonGroup>
                </div>
            </form>
        </div>

        <div class="flex-grow"/>

        <Button color="primary" disabled={!isConnected || isSending} on:click={() => send()}>
            {#if (isSending)}
                <Spinner/>
            {:else}
                Send all
            {/if}
        </Button>
    {:else}
        <div class="flex-grow"/>
        <Button color="primary" on:click={() => addDevice()}>
            Connect
        </Button>
    {/if}
</div>