import { type Writable, writable } from 'svelte/store';

export type State = {
	loading: Writable<boolean>;
	toast: Writable<{
		message: string;
		type: 'success' | 'error';
		timeToHide?: number;
	} | null>;
};

export default {
	loading: writable(false),
	toast: writable(null)
} as State;
