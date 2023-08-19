<script>
    import {Spinner} from "flowbite-svelte";
    import store from "$lib/store";

    let isLoading = false;

    store.loading.subscribe(
        loading => isLoading = loading
    );

</script>

{#if isLoading}
    <div class="absolute h-screen w-screen flex items-center justify-center bg-gray-900 bg-opacity-50 dark:bg-opacity-80">
        <Spinner/>
    </div>
{/if}