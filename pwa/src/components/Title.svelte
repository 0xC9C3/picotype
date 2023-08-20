<script lang="ts">
    import {Icon} from "flowbite-svelte-icons";
    import {Button} from "flowbite-svelte";
    import ConnectDevice from "./ConnectDevice.svelte";
    import {page} from '$app/stores';
</script>

<div class="w-full flex gap-4 justify-between items-center max-w-full overflow-hidden js">

    <ConnectDevice/>

    {#if $page.url.pathname === '/settings'}
        <Button color="alternative" href="/" size="sm">
            <Icon name="arrow-left-outline"/>
        </Button>
    {:else }
        <Button color="alternative" href="/settings" size="sm">
            <Icon name="user-settings-outline"/>
        </Button>
    {/if}
</div>