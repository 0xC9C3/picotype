<script>
    import {Button, Checkbox, CloseButton, Input, Label, Spinner} from "flowbite-svelte";
    import {settings} from "$lib/settings";
    import {Icon} from "flowbite-svelte-icons";
    import store from "$lib/store";
    import {log} from "$lib/log";

    let currentPackage = {
        // eslint-disable-next-line no-undef
        version: __APP_VERSION__
    };

    let lastDevice = settings.get('last_device');
    const removeLastDevice = () => {
        settings.remove('last_device');
        lastDevice = null;
    }

    let form = {
        autoConnect: settings.get('auto_connect') === 'true',
        debugMode: settings.get('debug_mode') === 'true',
    };

    const saveSettings = () => {
        settings.set('auto_connect', form.autoConnect);
        settings.set('debug_mode', form.debugMode);

        store.toast.set({
            message: 'Settings saved',
            type: 'success',
            duration: 2000
        })

        history.back();
    }

    let checkingForUpdates = false;
    const checkForUpdates = () => {
        if (checkingForUpdates) {
            return;
        }

        checkingForUpdates = true;
        fetch('https://api.github.com/repos/0xC9C3/picotype/releases/latest')
            .then(response => {
                if (!response.ok) {
                    log.e(response);
                    throw new Error('Network response was not ok');
                }

                return response.json();
            })
            .then(data => {
                if (data.tag_name !== undefined && data.tag_name !== currentPackage.version) {
                    store.toast.set({
                        message: 'Update available',
                        type: 'success',
                        duration: 2000
                    })
                } else {
                    store.toast.set({
                        message: 'No updates available',
                        type: 'info',
                        duration: 2000
                    })
                }
            })
            .catch(error => {
                log.e(error);
                store.toast.set({
                    message: 'Error checking for updates',
                    type: 'error',
                    duration: 2000
                })
            })
            .finally(() => {
                checkingForUpdates = false;
            });
    }
</script>

<div class="flex flex-col h-full">
    <!-- list devices remove / rename / add -->
    <div class="mb-3">
        <Label class="space-y-2">
            <span>Last Device</span>
            <Input readonly size="lg" type="email" value={lastDevice ?? 'None'}>
                <Icon class="w-6 h-6" name="link-solid" slot="left"/>
                <CloseButton on:click={() => removeLastDevice()} slot="right"/>
            </Input>
        </Label>
    </div>

    <!-- edit forms -->


    <!-- auto connect on start y/n -->
    <div class="mb-3">
        <Checkbox bind:checked={form.autoConnect}>Auto connect on start</Checkbox>
    </div>

    <!-- debug mode y/n -->
    <div class="mb-3">
        <Checkbox bind:checked={form.debugMode}>Debug mode</Checkbox>
    </div>

    <!-- console if debug mode -->
    {#if (settings.get('debugMode'))}
        <textarea class="flex-grow p-2 bg-gray-800 text-white mb-3"></textarea>
    {/if}


    <div class="flex-grow"></div>

    <!-- update check -->
    <div class="mb-3 gap-4 flex justify-between">
        <Button class="w-full" disabled={checkingForUpdates} on:click={() => checkForUpdates()} size="sm">
            <Icon class="w-3 h-3 mr-2" name="arrows-repeat-solid"/>
            {#if (!checkingForUpdates)}
                Check for updates
            {:else}
                <Spinner class="w-3 h-3 mr-2"/>
            {/if}
        </Button>
        <Button class="w-full" href="/docs.html" rel="external" size="sm">
            <Icon class="w-3 h-3 mr-2" name="book-solid"/>
            Documentation
        </Button>
    </div>

    <div class="pb-6 text-center">
        <a class="underline" href="https://github.com/0xC9C3/picotype">picotype</a> {currentPackage.version} by <a
            class="underline" href="https://github.com/0xC9C3/">0xC9C3</a>
    </div>

    <Button class="mb-4" color="primary" on:click={() => saveSettings()}>Save</Button>
    <Button color="alternative" on:click={() => history.back()}>Cancel</Button>
</div>