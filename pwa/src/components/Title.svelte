<script lang="ts">
    import {Icon} from "flowbite-svelte-icons";
    import {Button} from "flowbite-svelte";
    import ConnectDevice from "./ConnectDevice.svelte";
</script>

<div class="w-full flex gap-4 justify-between items-center max-w-full overflow-hidden js">

    <ConnectDevice/>

    <Button color="alternative" href="/settings" size="sm">
        <Icon name="user-settings-outline"/>
    </Button>
</div>