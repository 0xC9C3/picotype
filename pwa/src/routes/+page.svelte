<script lang="ts">
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

    const sendClipboard = async () => {
        if (!navigator.clipboard) {
            store.toast.set({
                message: "Clipboard not supported",
                type: "error"
            });
            return;
        }

        try {
            const text = await navigator.clipboard.readText();
            log.l("Clipboard text: " + text);
            await sendText(text);
        } catch (e) {
            store.toast.set({
                message: e.message,
                type: "error"
            });

            log.e(e);
        }
    }

    const sendText = async (text: string) => {
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

        log.l("Sending all");

        const textsToType = [
            username,
            password,
        ];
        for (let i = 0; i < textsToType.length; i++) {
            const text = textsToType[i];
            await sendText(text);

            // send tab between strokes if not last
            if (i < textsToType.length - 1) {
                await sendText("\t");
            }
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

    {#if isConnected || true}
        <form class="h-full flex flex-col" method="POST" on:submit|preventDefault={() => send()}>
            <div class="h-full flex flex-col justify-center">
                <div class="mb-3">
                    <Label class="mb-2" for="username">Username / E-Mail</Label>
                    <ButtonGroup class="w-full">
                        <Input name="username" autocomplete="username" autocorrect="off" bind:value={username}
                               id="username"
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
                        <Input name="password" autocomplete="password" autocorrect="off" autofocus="autofocus"
                               bind:value={password}
                               id="password"
                               placeholder="•••••••••"
                               type="password"/>
                        <Button on:click={() => sendText(password)} color="primary">
                            <Icon name="arrow-right-outline"/>
                        </Button>
                    </ButtonGroup>
                </div>
            </div>

            <div class="flex-grow"/>

            <Button class="mb-3" color="alternative" disabled={!isConnected || isSending} on:click={() => sendClipboard()}>
                {#if (isSending)}
                    <Spinner/>
                {:else}
                    Send clipboard
                {/if}
            </Button>

            <Button color="primary" disabled={!isConnected || isSending} type="submit">
                {#if (isSending)}
                    <Spinner/>
                {:else}
                    Send all
                {/if}
            </Button>
        </form>
    {:else}
        <div class="flex-grow"/>
        <Button color="primary" on:click={() => addDevice()}>
            Connect
        </Button>
    {/if}
</div>