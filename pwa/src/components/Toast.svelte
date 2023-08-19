<script lang="ts">
    import {Icon} from 'flowbite-svelte-icons';
    import {Toast} from "flowbite-svelte";
    import {slide} from "svelte/transition";
    import store from "$lib/store";


    let show = false;
    let timeToHide = 5000;
    let message = '';
    let type = 'success';

    store.toast.subscribe((toast) => {
        if (!toast) return;

        show = true;
        timeToHide = toast.timeToHide ?? 5000;
        message = toast.message ?? '';
        type = toast.type ?? 'success';

        setTimeout(() => {
            show = false;
            store.toast.set(null);
        }, timeToHide);
    });
</script>

<div class="absolute flex">
    <Toast bind:open={show} transition={slide}>
        <div class="flex">
            {#if (type === 'success')}
                <Icon class="w-5 h-5 text-green-500 dark:text-green-200"
                      name="check-outline"
                      slot="icon"/>
            {:else if (type === 'error')}
                <Icon class="w-5 h-5 text-red-500 dark:text-red-200" name="close-outline"
                      slot="icon"/>
            {:else if (type === 'warning')}
                <Icon class="w-5 h-5 text-yellow-500 dark:text-yellow-200"
                      name="alert-circle-outline" slot="icon"/>
            {:else if (type === 'info')}
                <Icon class="w-5 h-5 text-blue-500 dark:text-blue-200"
                      name="info-circle-outline"
                      slot="icon"/>
            {:else if (type === 'loading')}
                <Icon class="w-5 h-5 text-primary-500 dark:text-primary-200"
                      name="refresh-outline" slot="icon"/>
            {/if}

            <p class="ml-3">
                {message}
            </p>
        </div>
    </Toast>
</div>