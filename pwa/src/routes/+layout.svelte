<script>
    import '../app.postcss';
    import Title from "../components/Title.svelte";
    import {ble} from "$lib/ble";
    import LoadingOverlay from "../components/LoadingOverlay.svelte";
    import Toast from "../components/Toast.svelte";
</script>

<svelte:head>
    <title>picotype</title>
</svelte:head>

<div class="h-screen bg-teal-950 text-gray-100 flex justify-center items-center flex-col p-3">
    <div class="w-full mb-3">
        <Title/>
    </div>

    <div class="flex w-full flex-grow">
        <div class="col-span-10 bg-teal-900 rounded p-3 flex-grow">
            {#if ble.isSupported}
                <slot/>
            {:else}
                <div class="text-center">
                    <p class="text-2xl">Bluetooth not supported</p>
                </div>
            {/if}
        </div>
    </div>

    <LoadingOverlay/>
    <Toast/>
</div>

